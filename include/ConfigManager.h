#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <MD5Builder.h>
#include "WiFi.h"
#include "ConfigDefines.h"

struct RuntimeConfig {
    struct {
        char name[32];
        char firmwareVersion[16];
        uint64_t chipID;
        char macAddress[18];
        uint32_t statusUpdateInterval;
    } device;

    struct {
        char ssid[32];
        char password[64];
        bool autoReconnect;
        uint8_t maxConnectionAttempts;
        uint32_t reconnectInterval;
        uint32_t checkInterval;
    } wifi;

    struct {
        char broker[64];
        uint16_t port;
        char user[32];
        char password[64];
        uint32_t retryInterval;
        char baseTopic[64];
        uint8_t maxConnectionAttempts;
    } mqtt;

    struct {
        char serviceUUID[37];
        char charUUID[37];
        uint32_t timeout;
        uint8_t maxConnections;
    } bluetooth;
    
    struct {
        uint8_t maxRecoveryAttempts;
        uint32_t recoveryInterval;
    } error;

    struct {
        bool allowMqttLog;
        char mqttTopic[64];
        uint8_t logLevel;
    } logging;

    struct {
        char apiUrl[128];
        char apiToken[128];
        uint32_t interval;
        bool initialCheck;
    } update;

    char hash[33];
};

class ConfigManager {
private:
    ConfigManager() : initialized(false) {
        loadDefaults();
    }
    
    RuntimeConfig config;
    static constexpr const char* CONFIG_FILE = "/config.bin";
    bool initialized;

    String calculateHash(RuntimeConfig* config);
    bool loadFromFlash();
    bool saveToFlash();
    void loadDefaults();
    void setConfigFromDefines(RuntimeConfig* config);

public:
    ConfigManager(const ConfigManager&) = delete;
    void operator=(const ConfigManager&) = delete;

    static ConfigManager& getInstance(){
        static ConfigManager instance;
        return instance;
    }
    bool begin();
    RuntimeConfig& getRuntimeConfig() { return config; }
    bool hasConfigDefinesChanged();
    void updateDeviceConfig();
    void print(RuntimeConfig* config);
};

#endif