#include "managers/WifiManager.h"

const char *WifiManager::getWifiStatusString(WiFiStatus status)
{
    switch (status)
    {
    case WiFiStatus::DISCONNECTED:
        return "DISCONNECTED";
    case WiFiStatus::CONNECTING:
        return "CONNECTING";
    case WiFiStatus::CONNECTED:
        return "CONNECTED";
    case WiFiStatus::CONNECTION_FAILED:
        return "CONNECTION_FAILED";
    case WiFiStatus::WRONG_PASSWORD:
        return "WRONG_PASSWORD";
    case WiFiStatus::NO_SSID_AVAILABLE:
        return "NO_SSID_AVAILABLE";
    default:
        return "UNKNOWN";
    }
};

bool WifiManager::begin()
{

    if (strlen(WIFI_SSID) == 0)
    {
        log.warning("WifiManager", "No SSID available, skipping WifiManager initialization");
        return false;
    }
    else if (strlen(WIFI_PASSWORD) == 0)
    {
        log.warning("WifiManager", "No password available, skipping WifiManager initialization");
        return false;
    }

    ftmSemaphore = xSemaphoreCreateBinary();
    WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);

    WiFi.mode(WIFI_STA);
    return true;
}

bool WifiManager::isConnected()
{
    return status == WiFiStatus::CONNECTED && WiFi.status() == WL_CONNECTED;
}

bool WifiManager::connect()
{
    if (status == WiFiStatus::CONNECTED)
    {
        return true;
    }
    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Attempting to connect to Wifi-AP '%s' (['%s', %d], ['%s', %d])", WIFI_SSID, WIFI_SSID, strlen(WIFI_SSID), WIFI_PASSWORD, strlen(WIFI_PASSWORD));
    log.debug("WifiManager", msgBuffer);

    // WiFi.setMinSecurity(WIFI_AUTH_WEP);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    status = WiFiStatus::CONNECTING;
    lastAttempt = millis();
    connectionAttempts = 1;

    return true;
}

void WifiManager::disconnect()
{
    log.debug("WifiManager", "Disconnecting from Wifi...");

    WiFi.disconnect();
    status = WiFiStatus::DISCONNECTED;
}

void WifiManager::update()
{
    if (status == WiFiStatus::CONNECTING)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            status = WiFiStatus::CONNECTED;
            connectionAttempts = 0;

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Connected to Wifi-AP with IP: %s", WiFi.localIP().toString().c_str());
            log.info("WifiManager", buffer);

            return;
        }

        if (millis() - lastAttempt >= WIFI_CHECK_INTERVAL)
        {
            lastAttempt = millis();
            connectionAttempts++;

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Connection attempt %d/%d", connectionAttempts, WIFI_MAX_CONNECTION_ATTEMPTS);
            log.debug("WifiManager", buffer);

            if (connectionAttempts >= WIFI_MAX_CONNECTION_ATTEMPTS)
            {
                status = WiFiStatus::CONNECTION_FAILED;
                char msgBuffer[192];
                snprintf(msgBuffer, sizeof(msgBuffer), "Failed to connect to Wifi-AP ('%s') due reaching max connection attempts", WIFI_SSID);
                log.error("WifiManager", msgBuffer);

                return;
            }
        }
    }
    else if (status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED)
    {
        status = WiFiStatus::DISCONNECTED;
        char msgBuffer[192];
        snprintf(msgBuffer, sizeof(msgBuffer), "Lost connection to Wifi-AP ('%s')", WIFI_SSID);
        log.warning("WifiManager", msgBuffer);

        if (WIFI_AUTO_RECONNECT && millis() - lastAttempt >= WIFI_RECONNECT_INTERVAL)
        {
            connect();
        }
    }
}

WiFiStatus WifiManager::getStatus()
{
    return status;
}

String WifiManager::getIP()
{
    IPAddress localIP = WiFi.localIP();
    return String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + "." + String(localIP[3]);
}

String WifiManager::getSSID()
{
    return WiFi.SSID();
}

uint8_t *WifiManager::getBSSID()
{
    return WiFi.BSSID();
}

int32_t WifiManager::getRSSI()
{
    return WiFi.RSSI();
}

uint8_t WifiManager::getConnectionAttempts()
{
    return connectionAttempts;
}
