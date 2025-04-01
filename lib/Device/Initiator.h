#ifndef Initiator_H
#define Initiator_H

#include "LogManager.h"
#include "MQTTManager.h"
#include "WifiManager.h"
#include "dw3000.h"

class Initiator
{
private:
    Initiator(MQTTManager &mqttMgr, LogManager &logMgr, WifiManager &wifiMgr)
        : mqttManager(mqttMgr), logManager(logMgr), wifiManager(wifiMgr)
    {
    }

    MQTTManager &mqttManager;
    LogManager &logManager;
    WifiManager &wifiManager;

public:
    Initiator(const Initiator &) = delete;
    Initiator &operator=(const Initiator &) = delete;
    static Initiator &getInstance()
    {
        static Initiator instance(MQTTManager::getInstance(),
                                  LogManager::getInstance(),
                                  WifiManager::getInstance());
        return instance;
    }

    MQTTManager &getMQTTManager() { return mqttManager; }
    LogManager &getLogManager() { return logManager; }
    WifiManager &getWifiManager() { return wifiManager; }

    bool begin();      /// initiieren
    void run_tag();    // transmiter
    void run_anchor(); // receiver
};

#endif // Initiator_H
