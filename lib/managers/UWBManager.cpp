#include "UWBManager.h"

dwt_config_t config = {
    5,                /* Channel number. */
    DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    9,                /* TX preamble code. Used in TX only. */
    9,                /* RX preamble code. Used in RX only. */
    1,                /* 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2
                         for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
    DWT_BR_6M8,       /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    DWT_PHRRATE_STD,  /* PHY header rate. */
    (129 + 8 - 8),    /* SFD timeout (preamble length + 1 + SFD length - PAC
                         size).    Used in RX only. */
    DWT_STS_MODE_OFF, /* STS disabled */
    DWT_STS_LEN_64,   /* STS length see allowed values in Enum
                       * dwt_sts_lengths_e
                       */
    DWT_PDOA_M0       /* PDOA mode off */
};
extern dwt_txconfig_t txconfig_options;

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
uint32_t t_reply_1[NUM_DW3000 - 1];
uint64_t t_reply_2;
uint64_t t_round_1[NUM_DW3000 - 1];
uint32_t t_round_2[NUM_DW3000 - 1];
double tof, distance;
unsigned long previous_debug_millis = 0;
unsigned long current_debug_millis = 0;
int millis_since_last_serial_print;
uint32_t tx_time;
uint64_t tx_ts;

int target_uids[NUM_DW3000 - 1];

void set_target_uids()
{
/*
 * U1 is the initiator, U2 - U6 are responders
 * U1 - U6 are the target UIDs
 */
#ifdef TAG
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U6;
    case 5:
        target_uids[3] = U5;
    case 4:
        target_uids[2] = U4;
    case 3:
        target_uids[1] = U3;
    case 2:
        target_uids[0] = U2;
    default:
        break;
    }
#elif defined(ANCHOR_U1)
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U6;
    case 5:
        target_uids[3] = U5;
    case 4:
        target_uids[2] = U4;
    case 3:
        target_uids[1] = U3;
    case 2:
        target_uids[0] = U1;
    default:
        break;
    }
#elif defined(ANCHOR_U2)
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U6;
    case 5:
        target_uids[3] = U5;
    case 4:
        target_uids[2] = U4;
    case 3:
        target_uids[1] = U2;
    case 2:
        target_uids[0] = U1;
    default:
        break;
    }
#elif defined(ANCHOR_U3)
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U6;
    case 5:
        target_uids[3] = U5;
    case 4:
        target_uids[2] = U3;
    case 3:
        target_uids[1] = U2;
    case 2:
        target_uids[0] = U1;
    default:
        break;
    }
#elif defined(ANCHOR_U4)
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U6;
    case 5:
        target_uids[3] = U4;
    case 4:
        target_uids[2] = U3;
    case 3:
        target_uids[1] = U2;
    case 2:
        target_uids[0] = U1;
    default:
        break;
    }
#elif defined(ANCHOR_U5)
    switch (NUM_DW3000)
    {
    case 6:
        target_uids[4] = U5;
    case 5:
        target_uids[3] = U4;
    case 4:
        target_uids[2] = U3;
    case 3:
        target_uids[1] = U2;
    case 2:
        target_uids[0] = U1;
    default:
        break;
    }
#endif
}

void UWBManager::begin()
{
    spiBegin(UWB_IRQ, UWB_RST);
    spiSelect(UWB_SS);

    while (!dwt_checkidlerc())
    {
        log.error("UWBManager", "IDLE FAILED");
        while (1)
            ;
    }

    if (dwt_initialise(DWT_DW_INIT) == DWT_ERROR)
    {
        log.error("UWBManager", "IDLE FAILED");
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
    char msg[256];
    snprintf(msg, sizeof(msg), "Setup complete. Device: %s, UID: %d", DEVICE_NAME, UID);
    log.info("UWBManager", msg);
}

void UWBManager::initiator(char *payload)
{
    if (!wait_ack && !wait_final && (counter == 0))
    {
        // Starte einen neuen Ranging-Zyklus: Poll-Nachricht senden
        initiatorSendPoll();
    }
    else
    {
        // Falls bereits ein Vorgang läuft: RX-Modus aktivieren
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
        dwt_rxenable(DWT_START_RX_IMMEDIATE);
    }

    // Auf Empfang einer Nachricht warten (ACK oder Final)
    uint32_t status = waitForReceptionEvent(SYS_STATUS_RXFCG_BIT_MASK |
                                            SYS_STATUS_ALL_RX_TO |
                                            SYS_STATUS_ALL_RX_ERR);

    if (status & SYS_STATUS_RXFCG_BIT_MASK)
    {
        // Nachricht wurde erfolgreich empfangen
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);
        dwt_readrxdata(rx_buffer, BUF_LEN, 0);
        if (rx_buffer[MSG_SID_IDX] != target_uids[counter])
        {
            // Falscher Absender, Vorgang zurücksetzen
            dwt_write32bitreg(SYS_STATUS_ID,
                              SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
            counter = 0;
            wait_ack = false;
            wait_final = false;
            return;
        }
        // Verarbeite empfangene ACK bzw. Final-Nachricht
        if (wait_ack)
        {
            poll_tx_ts = get_tx_timestamp_u64();
            t_round_1[counter] = get_rx_timestamp_u64() - poll_tx_ts;
            resp_msg_get_ts(&rx_buffer[MSG_T_REPLY_IDX], &t_reply_1[counter]);
            ++counter;
        }
        else
        {
            resp_msg_get_ts(&rx_buffer[MSG_T_REPLY_IDX], &t_round_2[counter]);
            ++counter;
        }
    }
    else
    {
        // Fehler oder Timeout: Sende Reset-Nachricht und setze Status zurück
        tx_msg[MSG_SN_IDX] = frame_seq_nb;
        tx_msg[MSG_FUNC_IDX] = FUNC_CODE_RESET;
        sendTxMessage(tx_msg, MSG_LEN, DWT_START_TX_IMMEDIATE);
        dwt_write32bitreg(SYS_STATUS_ID,
                          SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        wait_ack = false;
        wait_final = false;
        counter = 0;
        Sleep(1);
        return;
    }
    // Übergang vom Poll- zum Range-Modus
    if (wait_ack && (counter == NUM_DW3000 - 1))
    {
        initiatorSendRange();
        return;
    }
    // Verarbeitung der Final-Nachrichten: Distanzberechnung durchführen
    if (wait_final && (counter == NUM_DW3000 - 1))
    {
        range_tx_ts = get_tx_timestamp_u64();
        current_debug_millis = millis();
        // Serial.print(current_debug_millis - previous_debug_millis);
        // Serial.print("ms\t");
        JsonDocument doc;
        JsonArray cellsArray = doc["distances"].to<JsonArray>();
        for (int i = 0; i < counter; i++)
        {
            t_reply_2 = range_tx_ts - (t_round_1[i] + poll_tx_ts);
            tof = (t_round_1[i] * t_round_2[i] - t_reply_1[i] * t_reply_2) /
                  (t_round_1[i] + t_round_2[i] + t_reply_1[i] + t_reply_2) *
                  DWT_TIME_UNITS;
            distance = tof * SPEED_OF_LIGHT;
            JsonObject nodeObj = cellsArray.add<JsonObject>();
            nodeObj["node_id"] = target_uids[i];
            nodeObj["distance_m"] = distance;
            snprintf(dist_str, sizeof(dist_str), "%3.3f m\t", distance);
            // Serial.print(target_uids[i]);
            // Serial.print("\t");
            // Serial.print(dist_str);
        }
        // Serial.println();
        char buffer[1024];
        serializeJson(doc, buffer);
        strcpy(payload, buffer);
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
    // RX-Modus sofort aktivieren
    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    uint32_t status = waitForReceptionEvent(SYS_STATUS_RXFCG_BIT_MASK |
                                            SYS_STATUS_ALL_RX_ERR);

    if (status & SYS_STATUS_RXFCG_BIT_MASK)
    {
        processResponderMessage();
    }
    else
    {
        // Bei Fehler oder Timeout: Status zurücksetzen und Poll-Modus aktivieren
        wait_poll = true;
        wait_range = false;
        counter = 0;
        dwt_write32bitreg(SYS_STATUS_ID,
                          SYS_STATUS_ALL_RX_ERR | SYS_STATUS_ALL_RX_TO);
        return;
    }

    if (wait_poll)
    {
        if (counter == WAIT_NUM)
        {
            responderSendAck();
        }
        if (counter == NUM_DW3000 - 1)
        {
            // Wechsel in den Range-Modus, wenn alle ACKs empfangen wurden
            counter = 0;
            wait_poll = false;
            wait_range = true;
            return;
        }
    }
    if (wait_range)
    {
        if (counter == WAIT_NUM)
        {
            responderSendFinal();
        }
        if (counter == NUM_DW3000 - 1)
        {
            // Alle Final-Nachrichten empfangen: Zurücksetzen für den nächsten Zyklus
            counter = 0;
            wait_poll = true;
            wait_range = false;
            ++frame_seq_nb;
            return;
        }
    }
}

float *UWBManager::getDistances()
{
    return distances;
}

// Wartet, bis eines der angegebenen RX-Ereignisse (Nachricht empfangen, Timeout, Fehler)
// auftritt, und gibt dann den Status zurück.
uint32_t waitForReceptionEvent(uint32_t eventMask)
{
    uint32_t status;
    while (!((status = dwt_read32bitreg(SYS_STATUS_ID)) & eventMask))
    {
        // Warten...
    }
    return status;
}
// Versendet eine Nachricht und wartet, bis die Übertragung abgeschlossen ist.
// Gibt true zurück, wenn der Sendevorgang erfolgreich war.
bool sendTxMessage(uint8_t *msg, uint16_t len, uint32_t txFlags)
{
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
    dwt_writetxdata(len, msg, 0);
    dwt_writetxfctrl(len, 0, 1);
    int ret = dwt_starttx(txFlags);
    if (ret == DWT_SUCCESS)
    {
        while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS_BIT_MASK))
        {
            // Warte auf Abschluss
        }
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
        return true;
    }
    return false;
}

// ===================== Initiator-spezifische Funktionen =====================

// Sendet eine Poll-Nachricht, um den Ranging-Vorgang zu starten.
void initiatorSendPoll()
{
    wait_ack = true;
    tx_msg[MSG_SN_IDX] = frame_seq_nb;
    tx_msg[MSG_FUNC_IDX] = FUNC_CODE_POLL;
    sendTxMessage(tx_msg, MSG_LEN, DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
}

// Sendet die Range-Nachricht, nachdem alle ACKs empfangen wurden.
void initiatorSendRange()
{
    // Berechne den verzögerten TX-Zeitpunkt
    tx_time = (get_rx_timestamp_u64() + (RX_TO_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    tx_ts = (((uint64_t)(tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
    dwt_setdelayedtrxtime(tx_time);
    tx_msg[MSG_SN_IDX] = frame_seq_nb;
    tx_msg[MSG_FUNC_IDX] = FUNC_CODE_RANGE;
    sendTxMessage(tx_msg, BUF_LEN, DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
    wait_ack = false;
    wait_final = true;
    counter = 0;
}
// ===================== Responder-spezifische Funktionen =====================

// Sendet eine ACK-Nachricht als Antwort auf einen Poll.
void responderSendAck()
{
    tx_time = (get_rx_timestamp_u64() + (RX_TO_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    tx_ts = (((uint64_t)(tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
    dwt_setdelayedtrxtime(tx_time);
    tx_msg[MSG_SN_IDX] = frame_seq_nb;
    tx_msg[MSG_FUNC_IDX] = FUNC_CODE_ACK;
    // Setzt den Zeitstempel-Differenzwert in das ACK-Paket
    resp_msg_set_ts(&tx_msg[MSG_T_REPLY_IDX], tx_ts - poll_rx_ts);
    sendTxMessage(tx_msg, BUF_LEN, DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
}

// Sendet eine Final-Nachricht als Antwort auf eine Range-Nachricht.
void responderSendFinal()
{
    ack_tx_ts = get_tx_timestamp_u64();
    tx_time = (get_rx_timestamp_u64() + (RX_TO_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    tx_ts = (((uint64_t)(tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
    dwt_setdelayedtrxtime(tx_time);
    // Setzt den Zeitstempel-Differenzwert in das Final-Paket
    resp_msg_set_ts(&tx_msg[MSG_T_REPLY_IDX], range_rx_ts - ack_tx_ts);
    tx_msg[MSG_SN_IDX] = frame_seq_nb;
    tx_msg[MSG_FUNC_IDX] = FUNC_CODE_FINAL;
    sendTxMessage(tx_msg, BUF_LEN, DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
}
// Liest und verarbeitet eine empfangene Nachricht im Responder.
void processResponderMessage()
{
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);
    dwt_readrxdata(rx_buffer, BUF_LEN, 0);

    // Bei einem Reset-Befehl: Setze den Poll-Modus und Zähler zurück
    if (rx_buffer[MSG_FUNC_IDX] == FUNC_CODE_RESET)
    {
        wait_poll = true;
        wait_range = false;
        counter = 0;
        return;
    }
    // Überprüfe, ob die Nachricht von dem erwarteten Absender stammt
    if (rx_buffer[MSG_SID_IDX] != target_uids[counter])
    {
        dwt_write32bitreg(SYS_STATUS_ID,
                          SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        wait_poll = true;
        wait_range = false;
        counter = 0;
        return;
    }
    // Verarbeite empfangene Nachrichten je nach aktuellem Modus:
    if (wait_poll)
    {
        if (counter == 0)
        {
            poll_rx_ts = get_rx_timestamp_u64();
        }
        ++counter;
    }
    else if (wait_range)
    {
        if (counter == 0)
        {
            range_rx_ts = get_rx_timestamp_u64();
        }
        ++counter;
    }
}
