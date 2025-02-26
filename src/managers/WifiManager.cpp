#include "managers/WiFiManager.h"

const char* WiFiManager::getWifiStatusString(WiFiStatus status) {
    switch (status) {
        case WiFiStatus::DISCONNECTED: return "DISCONNECTED";
        case WiFiStatus::CONNECTING: return "CONNECTING";
        case WiFiStatus::CONNECTED: return "CONNECTED";
        case WiFiStatus::CONNECTION_FAILED: return "CONNECTION_FAILED";
        case WiFiStatus::WRONG_PASSWORD: return "WRONG_PASSWORD";
        case WiFiStatus::NO_SSID_AVAILABLE: return "NO_SSID_AVAILABLE";
        default: return "UNKNOWN";
    }
};

bool WiFiManager::begin(){
    log.debug("WiFiManager", "Initializing WiFiManager...");

    RuntimeConfig& config = configManager.getRuntimeConfig();

    if(strlen(config.wifi.ssid) == 0) {
        log.warning("WiFiManager", "No SSID available, skipping WiFiManager initialization");
        return false;
    } else if (strlen(config.wifi.password) == 0) {
        log.warning("WiFiManager", "No password available, skipping WiFiManager initialization");
        return false;
    } 

    WiFi.mode(WIFI_STA);
    return true;
}

bool WiFiManager::isConnected() {
    return status == WiFiStatus::CONNECTED && WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::connect() { 
    if(status == WiFiStatus::CONNECTED) {
        return true;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Attempting to connect to Wifi-AP '%s' (['%s', %d], ['%s', %d])", config.wifi.ssid, config.wifi.ssid, strlen(config.wifi.ssid), config.wifi.password, strlen(config.wifi.password));
    log.debug("WiFiManager", msgBuffer);

    //WiFi.setMinSecurity(WIFI_AUTH_WEP); 
    WiFi.begin(config.wifi.ssid, config.wifi.password);

    status = WiFiStatus::CONNECTING;
    lastAttempt = millis();
    connectionAttempts = 1;

    return true;
}

void WiFiManager::disconnect() {
    log.debug("WiFiManager", "Disconnecting from Wifi...");

    WiFi.disconnect();
    status = WiFiStatus::DISCONNECTED;
}

void WiFiManager::update(){
    RuntimeConfig &config = configManager.getRuntimeConfig();
    if(status == WiFiStatus::CONNECTING){
        if (WiFi.status() == WL_CONNECTED){
            status = WiFiStatus::CONNECTED;
            connectionAttempts = 0;

            char msgBuffer[64];
            snprintf(msgBuffer, sizeof(msgBuffer), "Connected to Wifi-AP with IP: %s", WiFi.localIP().toString().c_str());
            log.debug("WiFiManager", msgBuffer);

            return;
        }

        if (millis() - lastAttempt >= config.wifi.checkInterval) {
            lastAttempt = millis();
            connectionAttempts++;

            Serial.printf("Connection Attempts: %d (%d)\n", connectionAttempts, config.wifi.maxConnectionAttempts);
            if (connectionAttempts >= config.wifi.maxConnectionAttempts) {
                status = WiFiStatus::CONNECTION_FAILED;
                char msgBuffer[192];
                snprintf(msgBuffer, sizeof(msgBuffer), "Failed to connect to Wifi-AP ('%s') due reaching max connection attempts", config.wifi.ssid);
                log.error("WiFiManager", msgBuffer);

                return;
            }
        }
    } else if (status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED) {
        status = WiFiStatus::DISCONNECTED;
        char msgBuffer[192];
        snprintf(msgBuffer, sizeof(msgBuffer), "Lost connection to Wifi-AP ('%s')", config.wifi.ssid);
        log.warning("WiFiManager", msgBuffer);

        if (config.wifi.autoReconnect && millis() - lastAttempt >= config.wifi.reconnectInterval) {
            connect();
        }
    }
}

WiFiStatus WiFiManager::getStatus(){
    return status;
}

String WiFiManager::getIP() {
    IPAddress localIP = WiFi.localIP();
    return String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + "." + String(localIP[3]);
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

uint8_t* WiFiManager::getBSSID() {
    return WiFi.BSSID();
}

int32_t WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

uint8_t WiFiManager::getConnectionAttempts() {
    return connectionAttempts;
}

bool WiFiManager::ftmAP(const char* ssid){
    return WiFi.softAP(ssid, NULL, 1, 0, 4, true);
}