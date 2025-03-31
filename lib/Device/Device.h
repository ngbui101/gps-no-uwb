#ifndef DEVICE_H
#define DEVICE_H

#include "LogManager.h"
#include "MQTTManager.h"
#include "WifiManager.h"
#include "dw3000.h"

class Device
{
private:
    Device(MQTTManager &mqttMgr, LogManager &logMgr, WifiManager &wifiMgr)
        : mqttManager(mqttMgr), logManager(logMgr), wifiManager(wifiMgr)
    {
    }

    MQTTManager &mqttManager;
    LogManager &logManager;
    WifiManager &wifiManager;

public:
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
    static Device &getInstance()
    {
        static Device instance(MQTTManager::getInstance(),
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

#endif // DEVICE_H
