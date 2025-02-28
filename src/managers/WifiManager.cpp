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

    ftmSemaphore = xSemaphoreCreateBinary();
    WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);

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

uint8_t* WifiManager::getBSSID() {
    return WiFi.BSSID();
}

int32_t WifiManager::getRSSI() {
    return WiFi.RSSI();
}

uint8_t WifiManager::getConnectionAttempts() {
    return connectionAttempts;
}

bool WifiManager::ftmAP(const char* ssid){
    return WiFi.softAP(ssid, NULL, 1, 0, 4, true);
}

void WifiManager::onFtmReport(arduino_event_t *event) {
    
    const char *status_str[5] = {"SUCCESS", "UNSUPPORTED", "CONF_REJECTED", "NO_RESPONSE", "FAIL"};

    wifi_event_ftm_report_t *report = &event->event_info.wifi_ftm_report;
    WifiManager::getInstance().ftmSuccess = report->status == FTM_STATUS_SUCCESS;
    if (WifiManager::getInstance().ftmSuccess) {

        char msgBuffer[256];
        snprintf(msgBuffer, sizeof(msgBuffer), "FTM report status: %s, Distance: %.2f m, Return Time: %lu ns", status_str[report->status], (float)report->dist_est / 100.0, report->rtt_est);
        LogManager::getInstance().info("WiFiManager", msgBuffer);

        free(report->ftm_report_data);
    } else {
        char msgBuffer[64];
        snprintf(msgBuffer, sizeof(msgBuffer), "FTM report status: %s", status_str[report->status]);
        LogManager::getInstance().warning("WiFiManager", msgBuffer);
    }
    xSemaphoreGive(WifiManager::getInstance().ftmSemaphore);
}

bool WifiManager::initiateFtm(uint8_t channel, byte mac[]) {

    RuntimeConfig& config = configManager.getRuntimeConfig();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Initiating FTM session channnel: %d, mac: %02X:%02X:%02X:%02X:%02X:%02X, frameCount: %d, burstPeriod: %d ms",
                                    channel, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], config.wifi.ftmFrameCount, config.wifi.ftmBurstPeriod * 100);
    log.info("WiFiManager", msgBuffer);

    if (!WiFi.initiateFTM(config.wifi.ftmFrameCount, config.wifi.ftmBurstPeriod, channel, mac)) {
        log.error("WiFiManager", "Failed to initiate FTM session");
        return false;
    }
    
    return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;
}

int WifiManager::scan(bool ftm = false){
    int n = WiFi.scanNetworks();

    if (n == 0) {
        log.warning("WiFiManager", "No networks found");
    } else {
        char msgBuffer[256];
        snprintf(msgBuffer, sizeof(msgBuffer), "Found %d networks", n);
        log.info("WiFiManager", msgBuffer);

        log.info("WiFiManager", "| Nr | SSID                             | RSSI | CH | MAC               |");
        log.info("WiFiManager", "|----|----------------------------------|------|----|-------------------|");

        for (int i = 0; i < n; ++i) {

            if (ftm){
                initiateFtm(WiFi.channel(i), WiFi.BSSID(i));
            }

            snprintf(msgBuffer, sizeof(msgBuffer), "| %2d | %-32.32s | %4ld | %2ld | %02X:%02X:%02X:%02X:%02X:%02X | ",
                        i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i),
                        WiFi.BSSID(i)[0], WiFi.BSSID(i)[1], WiFi.BSSID(i)[2], WiFi.BSSID(i)[3], WiFi.BSSID(i)[4], WiFi.BSSID(i)[5]);
            log.info("WiFiManager", msgBuffer);
        }
    }

    WiFi.scanDelete();
    return n;
}