#ifndef UWB_MANAGER_H
#define UWB_MANAGER_H

#include "LogManager.h"
#include "ConfigDefines.h"
#include "dw3000.h"

const uint8_t MSG_LEN = 16;         // message length
const uint8_t BUF_LEN = MSG_LEN;    // buffer length
const uint8_t MSG_SN_IDX = 2;       // sequence number
const uint8_t MSG_SID_IDX = 7;      // source id
const uint8_t MSG_FUNC_IDX = 9;     // function code
const uint8_t MSG_T_REPLY_IDX = 10; // byte index of transmitter timestamp

constexpr uint16_t TX_TO_RX_DLY_UUS = 100; // delay from transmitter to receiver in microseconds
constexpr uint16_t RX_TO_TX_DLY_UUS = 800; // delay from receiver to transmitter in microseconds

// Funktion Codes
constexpr uint8_t FUNC_CODE_POLL = 0xE2;
constexpr uint8_t FUNC_CODE_ACK = 0xE3;
constexpr uint8_t FUNC_CODE_RANGE = 0xE4;
constexpr uint8_t FUNC_CODE_FINAL = 0xE5;
constexpr uint8_t FUNC_CODE_RESET = 0xE6;

constexpr uint32_t RX_TIMEOUT_UUS = 400000; // receiver timeout in microseconds

// UWBManager – Singleton zur Steuerung der UWB-Funktionalität
class UWBManager
{
private:
    // Privater Konstruktor (Singleton)
    UWBManager() : log(LogManager::getInstance()) {}
    LogManager &log;

public:
    /**
     * @brief Gibt die Singleton-Instanz von UWBManager zurück.
     *
     * @return Referenz auf die einzige Instanz von UWBManager.
     */
    static UWBManager &getInstance()
    {
        static UWBManager instance;
        return instance;
    }

    /**
     * @brief Startet den UWB-Betrieb.
     *
     * Führt alle notwendigen Initialisierungen für UWB durch.
     */
    void begin();

    /**
     * @brief Führt die Initiator-Routine aus.
     *
     * Diese Methode kann als Startpunkt für eine UWB-Initiator-Routine verwendet werden.
     */
    void initiator(double *tofArray);

    /**
     * @brief Führt die Responder-Routine aus.
     *
     * Diese Methode kann als Startpunkt für eine UWB-Responder-Routine verwendet werden.
     */
    void responder();
};

#endif
