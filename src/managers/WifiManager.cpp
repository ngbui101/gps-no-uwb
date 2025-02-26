#include "managers/WifiManager.h"

const char* WifiManager::getWifiStatusString(WiFiStatus status) {
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

bool WifiManager::begin(){
    log.debug("WifiManager", "Initializing WifiManager...");

    RuntimeConfig& config = configManager.getRuntimeConfig();

    if(strlen(config.wifi.ssid) == 0) {
        log.warning("WifiManager", "No SSID available, skipping WifiManager initialization");
        return false;
    } else if (strlen(config.wifi.password) == 0) {
        log.warning("WifiManager", "No password available, skipping WifiManager initialization");
        return false;
    } 

    WiFi.mode(WIFI_STA);
    return true;
}

bool WifiManager::isConnected() {
    return status == WiFiStatus::CONNECTED && WiFi.status() == WL_CONNECTED;
}

bool WifiManager::connect() { 
    if(status == WiFiStatus::CONNECTED) {
        return true;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Attempting to connect to Wifi-AP '%s' (['%s', %d], ['%s', %d])", config.wifi.ssid, config.wifi.ssid, strlen(config.wifi.ssid), config.wifi.password, strlen(config.wifi.password));
    log.debug("WifiManager", msgBuffer);

    //WiFi.setMinSecurity(WIFI_AUTH_WEP); 
    WiFi.begin(config.wifi.ssid, config.wifi.password);

    status = WiFiStatus::CONNECTING;
    lastAttempt = millis();
    connectionAttempts = 1;

    return true;
}

void WifiManager::disconnect() {
    log.debug("WifiManager", "Disconnecting from Wifi...");

    WiFi.disconnect();
    status = WiFiStatus::DISCONNECTED;
}

void WifiManager::update(){
    RuntimeConfig &config = configManager.getRuntimeConfig();
    if(status == WiFiStatus::CONNECTING){
        if (WiFi.status() == WL_CONNECTED){
            status = WiFiStatus::CONNECTED;
            connectionAttempts = 0;

            char msgBuffer[64];
            snprintf(msgBuffer, sizeof(msgBuffer), "Connected to Wifi-AP with IP: %s", WiFi.localIP().toString().c_str());
            log.debug("WifiManager", msgBuffer);

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
                log.error("WifiManager", msgBuffer);

                return;
            }
        }
    } else if (status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED) {
        status = WiFiStatus::DISCONNECTED;
        char msgBuffer[192];
        snprintf(msgBuffer, sizeof(msgBuffer), "Lost connection to Wifi-AP ('%s')", config.wifi.ssid);
        log.warning("WifiManager", msgBuffer);

        if (config.wifi.autoReconnect && millis() - lastAttempt >= config.wifi.reconnectInterval) {
            connect();
        }
    }
}

WiFiStatus WifiManager::getStatus(){
    return status;
}

String WifiManager::getIP() {
    IPAddress localIP = WiFi.localIP();
    return String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + "." + String(localIP[3]);
}

String WifiManager::getSSID() {
    return WiFi.SSID();
}

int32_t WifiManager::getRSSI() {
    return WiFi.RSSI();
}

uint8_t WifiManager::getConnectionAttempts() {
    return connectionAttempts;
}

