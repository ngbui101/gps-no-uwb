#include "UWBManager.h"
#include "dw3000.h"

extern dwt_txconfig_t txconfig_options;

// Konfiguration für den DW3000
dwt_config_t config = {
    5,                /* Channel number. */
    DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    9,                /* TX preamble code. Used in TX only. */
    9,                /* RX preamble code. Used in RX only. */
    1,                /* SFD type */
    DWT_BR_6M8,       /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    DWT_PHRRATE_STD,  /* PHY header rate. */
    (129 + 8 - 8),    /* SFD timeout */
    DWT_STS_MODE_OFF, /* STS disabled */
    DWT_STS_LEN_64,   /* STS length */
    DWT_PDOA_M0       /* PDOA mode off */
};

uint8_t tx_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 0, UID,
                    0, 0, 0, 0, 0, 0, 0, 0};
uint8_t rx_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 0, UID,
                    0, 0, 0, 0, 0, 0, 0, 0};
uint8_t frame_seq_nb = 0;
uint8_t rx_buffer[BUF_LEN];

uint32_t status_reg = 0;
bool wait_poll = true;
bool wait_final = false;
bool wait_ack = true;
bool wait_range = false;
int counter = 0;
int ret;

uint64_t poll_tx_ts, poll_rx_ts, range_tx_ts, ack_tx_ts, range_rx_ts;
uint32_t t_reply_1[NUM_DEVS - 1];
uint64_t t_reply_2;
uint64_t t_round_1[NUM_DEVS - 1];
uint32_t t_round_2[NUM_DEVS - 1];
double tof, distance;
unsigned long previous_debug_millis = 0;
unsigned long current_debug_millis = 0;
uint32_t tx_time;
uint64_t tx_ts;

int target_uids[NUM_DEVS - 1];

// Setzt die Ziel-UIDs je nach Gerätetyp (basierend auf NUM_DEVS statt NUM_NODES)
static void set_target_uids()
{
#ifdef TAG
    switch (NUM_DEVS)
    {
    case 4:
        target_uids[2] = ANCHORID_3;
    case 3:
        target_uids[1] = ANCHORID_2;
    case 2:
        target_uids[0] = ANCHORID_1;
        break;
    default:
        break;
    }
#elif defined(ANCHOR_1)
    switch (NUM_DEVS)
    {
    case 4:
        target_uids[2] = ANCHORID_3;
    case 3:
        target_uids[1] = ANCHORID_2;
    case 2:
        target_uids[0] = TAG_ID;
        break;
    default:
        break;
    }
#elif defined(ANCHOR_2)
    switch (NUM_DEVS)
    {
    case 4:
        target_uids[2] = ANCHORID_3;
    case 3:
        target_uids[1] = ANCHORID_1;
    case 2:
        target_uids[0] = TAG_ID;
        break;
    default:
        break;
    }
#elif defined(ANCHOR_3)
    switch (NUM_DEVS)
    {
    case 4:
        target_uids[2] = ANCHORID_2;
    case 3:
        target_uids[1] = ANCHORID_1;
    case 2:
        target_uids[0] = TAG_ID;
        break;
    default:
        break;
    }
#endif
}

void UWBManager::begin()
{
    spiBegin(UWB_IRQ, UWB_RST);
    spiSelect(UWB_SS);
    // Prüfe, ob DW3000 im Idle-Modus ist.
    while (!dwt_checkidlerc())
    {
        log.error("UWBManager", "IDLE FAILED");
        while (1)
            ; // Endlosschleife im Fehlerfall
    }

    if (dwt_initialise(DWT_DW_INIT) == DWT_ERROR)
    {
        log.error("UWBManager", "INIT FAILED");
        while (1)
            ;
    }

    dwt_setleds(DWT_LEDS_DISABLE);

    if (dwt_configure(&config))
    {
        log.error("UWBManager", "CONFIG FAILED");
        while (1)
            ;
    }

    dwt_configuretxrf(&txconfig_options);
    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);
    dwt_setrxaftertxdelay(TX_TO_RX_DLY_UUS);
#ifdef TAG
    dwt_setrxtimeout(RX_TIMEOUT_UUS);
#else
    dwt_setrxtimeout(0);
#endif
    dwt_setlnapamode(DWT_LNA_ENABLE | DWT_PA_ENABLE);

    set_target_uids();

    log.info("DEVICE", DEVICE_NAME);
    log.info("UWBManager", "Setup DW3000 successful");
}

void UWBManager::initiator(double *tofArray)
{
    // Initiator-Routine: Starte Messvorgang
    if (!wait_ack && !wait_final && (counter == 0))
    {
        wait_ack = true;
        tx_msg[MSG_SN_IDX] = frame_seq_nb;
        tx_msg[MSG_FUNC_IDX] = FUNC_CODE_POLL;
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
        dwt_writetxdata(MSG_LEN, tx_msg, 0);
        dwt_writetxfctrl(MSG_LEN, 0, 1);
        dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
        log.debug("UWBManager", "Poll message sent; starting measurement (Poll mode).");
    }
    else
    {
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
        dwt_rxenable(DWT_START_RX_IMMEDIATE);
        log.debug("UWBManager", "Poll already sent; enabling RX mode.");
    }

    // Warte auf Empfang einer ACK- oder Final-Nachricht
    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
             (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
    {
        ; // Warten
    }
    log.debug("UWBManager", "Message received (ACK/Final or timeout occurred).");

    if (status_reg & SYS_STATUS_RXFCG_BIT_MASK)
    {
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);
        dwt_readrxdata(rx_buffer, BUF_LEN, 0);
        log.debug("UWBManager", "Valid message received.");
        if (rx_buffer[MSG_SID_IDX] != target_uids[counter])
        {
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
            log.debug("UWBManager", "UID mismatch: Received target UID does not match expected value.");
            counter = 0;
            wait_ack = false;
            wait_final = false;
            return;
        }
        if (wait_ack)
        {
            poll_tx_ts = get_tx_timestamp_u64();
            t_round_1[counter] = get_rx_timestamp_u64() - poll_tx_ts;
            resp_msg_get_ts(&rx_buffer[MSG_T_REPLY_IDX], &t_reply_1[counter]);
            log.debug("UWBManager", "ACK message processed.");
            ++counter;
        }
        else
        {
            resp_msg_get_ts(&rx_buffer[MSG_T_REPLY_IDX], &t_round_2[counter]);
            log.debug("UWBManager", "Final message timestamp extracted.");
            ++counter;
        }
    }
    else
    {
        // Timeout oder Fehler: Sende RESET-Nachricht
        tx_msg[MSG_SN_IDX] = frame_seq_nb;
        tx_msg[MSG_FUNC_IDX] = FUNC_CODE_RESET;
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
        dwt_writetxdata(MSG_LEN, tx_msg, 0);
        dwt_writetxfctrl(MSG_LEN, 0, 1);
        dwt_starttx(DWT_START_TX_IMMEDIATE);
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        log.debug("UWBManager", "Timeout/Error: RESET message sent.");
        wait_ack = false;
        wait_final = false;
        counter = 0;
        Sleep(1);
        return;
    }
    // Wenn alle ACK-/Final-Nachrichten empfangen wurden (von (NUM_DEVS - 1) Anchors)
    if (wait_final && (counter == (NUM_DEVS - 1)))
    {
        range_tx_ts = get_tx_timestamp_u64();
        current_debug_millis = millis();
        char debugMsg[64];
        snprintf(debugMsg, sizeof(debugMsg), "%lu ms elapsed", current_debug_millis - previous_debug_millis);
        log.debug("UWBManager", debugMsg);
        // Für jeden Anchor den tof berechnen und in das übergebene Array schreiben
        for (int i = 0; i < counter; i++)
        {
            t_reply_2 = range_tx_ts - (t_round_1[i] + poll_tx_ts);
            double computed_tof = (t_round_1[i] * t_round_2[i] - t_reply_1[i] * t_reply_2) /
                                  (t_round_1[i] + t_round_2[i] + t_reply_1[i] + t_reply_2) * DWT_TIME_UNITS;
            tofArray[i] = computed_tof; // Speichere den berechneten tof in das Array
            double distance = computed_tof * SPEED_OF_LIGHT;
            char dist_str[32];
            snprintf(dist_str, sizeof(dist_str), "%d %3.3f m\t", target_uids[i], distance);
            log.debug("UWBManager", dist_str);
            // Serial.print(target_uids[i]);
            // Serial.print("\t");
            // Serial.print(dist_str);
        }
        // Serial.println();
        log.debug("UWBManager", "Final messages processed; measurement complete.");
        previous_debug_millis = current_debug_millis;
        counter = 0;
        wait_ack = false;
        wait_final = false;
        ++frame_seq_nb;
        Sleep(INTERVAL);
    }
}

void UWBManager::responder()
{
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
    // Warte auf Empfang einer Nachricht (oder Fehler)
    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
             (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_ERR)))
    {
        ;
    }

    if (status_reg & SYS_STATUS_RXFCG_BIT_MASK)
    { // Nachricht empfangen
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);
        dwt_readrxdata(rx_buffer, BUF_LEN, 0);
        // Bei RESET-Funktion wird der Zustand zurückgesetzt
        if (rx_buffer[MSG_FUNC_IDX] == FUNC_CODE_RESET)
        {
            wait_poll = true;
            wait_range = false;
            counter = 0;
            return;
        }
        // Überprüfe, ob die empfangene UID (im MSG_SID_IDX) mit dem erwarteten Wert übereinstimmt
        if (rx_buffer[MSG_SID_IDX] != target_uids[counter])
        {
            dwt_write32bitreg(SYS_STATUS_ID,
                              SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
            wait_poll = true;
            wait_range = false;
            counter = 0;
            return;
        }
        // Falls wir im Poll-Modus sind, verarbeite den Poll:
        if (wait_poll)
        {
            if (counter == 0)
            {
                poll_rx_ts = get_rx_timestamp_u64();
            }
            ++counter;
        }
        // Falls wir im Range-Modus sind:
        else if (wait_range)
        {
            if (counter == 0)
            {
                range_rx_ts = get_rx_timestamp_u64();
            }
            ++counter;
        }
    }
    else
    {
        // Bei Fehlern oder Timeout
        wait_poll = true;
        wait_range = false;
        counter = 0;
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR | SYS_STATUS_ALL_RX_TO);
        return;
    }

    // Poll-Modus: Falls alle erwarteten ACK-Nachrichten (von (NUM_DEVS - 1) Anchors) empfangen wurden
    if (wait_poll)
    {
        if (counter == (NUM_DEVS - 1))
        {
            tx_time = (get_rx_timestamp_u64() +
                       (RX_TO_TX_DLY_UUS * UUS_TO_DWT_TIME)) >>
                      8;
            tx_ts = (((uint64_t)(tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
            dwt_setdelayedtrxtime(tx_time);
            tx_msg[MSG_SN_IDX] = frame_seq_nb;
            tx_msg[MSG_FUNC_IDX] = FUNC_CODE_ACK;
            resp_msg_set_ts(&tx_msg[MSG_T_REPLY_IDX], tx_ts - poll_rx_ts);
            dwt_writetxdata((uint16_t)(BUF_LEN), tx_msg, 0);
            dwt_writetxfctrl((uint16_t)(BUF_LEN), 0, 1);
            ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
            if (ret == DWT_SUCCESS)
            {
                while (!(dwt_read32bitreg(SYS_STATUS_ID) &
                         SYS_STATUS_TXFRS_BIT_MASK))
                {
                    ;
                }
                dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
            }
        }
        // Wenn alle ACK-Nachrichten (von (NUM_DEVS - 1) Anchors) empfangen wurden:
        if (counter == (NUM_DEVS - 1))
        {
            counter = 0;
            wait_poll = false;
            wait_range = true;
            return;
        }
    }

    // Range-Modus: Verarbeitung der Final-Nachrichten
    if (wait_range)
    {
        if (counter == (NUM_DEVS - 1))
        {
            /* Sende Final Message */
            ack_tx_ts = get_tx_timestamp_u64(); // Zeitpunkt des ACK-Versands
            tx_time = (get_rx_timestamp_u64() +
                       (RX_TO_TX_DLY_UUS * UUS_TO_DWT_TIME)) >>
                      8;
            tx_ts = (((uint64_t)(tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
            dwt_setdelayedtrxtime(tx_time);
            resp_msg_set_ts(&tx_msg[MSG_T_REPLY_IDX], range_rx_ts - ack_tx_ts);
            tx_msg[MSG_SN_IDX] = frame_seq_nb;
            tx_msg[MSG_FUNC_IDX] = FUNC_CODE_FINAL;
            dwt_writetxdata((uint16_t)(BUF_LEN), tx_msg, 0);
            dwt_writetxfctrl((uint16_t)(BUF_LEN), 0, 1);
            ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
            if (ret == DWT_SUCCESS)
            {
                while (!(dwt_read32bitreg(SYS_STATUS_ID) &
                         SYS_STATUS_TXFRS_BIT_MASK))
                {
                    ;
                }
                dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
            }
        }
        if (counter == (NUM_DEVS - 1))
        {
            /* Alle Final-Nachrichten empfangen */
            counter = 0;
            wait_poll = true;
            wait_range = false;
            ++frame_seq_nb;
            return;
        }
    }
}
