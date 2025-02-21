#ifndef DEVICE_H
#define DEVICE_H

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 1
#define VERSION_STRING STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

#include "ConfigManager.h"
#include "states/DeviceState.h"
#include "MQTTManager.h"
#include "LogManager.h"
#include <ArduinoJson.h>

enum class DeviceStatus {
    BOOTING,
    IDLE,
    SETUP,
    TRANSITIONING,
    ACTION,
    ERROR,
    __DELIMITER__
};

class DeviceState;

class Device {
private:
    Device() 
        : currentState(nullptr)
        , lastStatusUpdate(0)
        , mqttManager(MQTTManager::getInstance())
        , configManager(ConfigManager::getInstance())
        , log(LogManager::getInstance()) {}
    
    MQTTManager& mqttManager;
    ConfigManager& configManager;
    LogManager& log;

    static const size_t JSON_DOC_SIZE = 512;
    DeviceState* currentState;
    uint32_t lastStatusUpdate;

    void sendDeviceStatus();

    const char* getDeviceStatusString(DeviceStatus status);
    constexpr size_t getDeviceStatusCount() {return static_cast<size_t>(DeviceStatus::__DELIMITER__);};

public:
    Device(const Device&) = delete;
    void operator=(const Device&) = delete;

    static Device& getInstance() {
        static Device instance;
        return instance;
    }

    void changeState(DeviceState& newState);
    void update();
    DeviceState* getCurrentState() { return currentState; }
};

#endif