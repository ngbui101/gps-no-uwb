#ifndef RESPONDER_H
#define RESPONDER_H

#include "LogManager.h"
#include "MQTTManager.h"
#include "WifiManager.h"
#include "UWBManager.h"

/**
 * @brief The Responder class coordinates the UWB responder functionality.
 *
 * This singleton class provides access to MQTTManager, LogManager, WifiManager, and UWBManager.
 * It offers methods for system initialization and for running the responder routine (runAnchor).
 */
class Responder
{
private:
    // Private constructor (singleton)
    Responder(MQTTManager &mqttMgr, LogManager &logMgr, WifiManager &wifiMgr, UWBManager &uwbMgr)
        : mqttManager(mqttMgr), logManager(logMgr), wifiManager(wifiMgr), uwbManager(uwbMgr)
    {
    }

    MQTTManager &mqttManager;
    LogManager &logManager;
    WifiManager &wifiManager;
    UWBManager &uwbManager;

public:
    // Delete copy constructor and assignment operator
    Responder(const Responder &) = delete;
    Responder &operator=(const Responder &) = delete;

    /**
     * @brief Returns the singleton instance of Responder.
     *
     * @return Reference to the unique Responder instance.
     */
    static Responder &getInstance()
    {
        static Responder instance(MQTTManager::getInstance(),
                                  LogManager::getInstance(),
                                  WifiManager::getInstance(),
                                  UWBManager::getInstance());
        return instance;
    }

    MQTTManager &getMQTTManager() { return mqttManager; }
    LogManager &getLogManager() { return logManager; }
    WifiManager &getWifiManager() { return wifiManager; }
    UWBManager &getUWBManager() { return uwbManager; }

    /**
     * @brief Initializes the responder.
     *
     * @return true if initialization is successful, false otherwise.
     */
    bool begin();

    /**
     * @brief Runs the responder routine.
     *
     * This method contains the code for handling incoming UWB messages.
     */
    void runAnchor();
};

#endif // RESPONDER_H
