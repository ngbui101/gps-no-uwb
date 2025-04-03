#include "WifiManager.h"

bool WifiManager::begin()
{
    WiFi.onEvent(WifiManager::handleWiFiEvent);
    return (WiFi.mode(WIFI_STA) && WiFi.disconnect());
}

bool WifiManager::connect()
{
    if (strlen(WIFI_SSID) == 0 || !isTargetSSIDFound(WIFI_SSID))
    {
        log.error("WifiManager", "SSID not available");
        return false;
    }
    if (strlen(WIFI_PASSWORD) == 0)
    {
        log.error("WifiManager", "No Password available");
        return false;
    }

    if (!WiFi.begin(WIFI_SSID, WIFI_PASSWORD))
    {
        char errorBuffer[128];
        snprintf(errorBuffer, sizeof(errorBuffer), "Fail to begin Wifi %s", WIFI_SSID);
        log.error("WifiManager", errorBuffer);
        return false;
    }

    long time_out = 60000; // 60s timeout
    long start_time = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        log.delay(500);

        if ((millis() - start_time) > time_out)
        {
            char errorBuffer[128];
            snprintf(errorBuffer, sizeof(errorBuffer), "Connect to %s fail, please check your pwd", WIFI_SSID);
            log.error("WifiManager", errorBuffer);
            return false;
        }
    }
    return true;
}

bool WifiManager::isTargetSSIDFound(const char *targetSSID)
{
    int n = WiFi.scanComplete();
    bool ssidFound = false;
    if (n < 0)
    {
        n = WiFi.scanNetworks();
    }
    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i) == String(targetSSID))
        {
            ssidFound = true;
            break;
        }
    }
    WiFi.scanDelete();
    return ssidFound;
}

void WifiManager::handleWiFiEvent(WiFiEvent_t event)
{
    // TODO: WiFi reconnection attempt not set.
    switch (event)
    {
    case SYSTEM_EVENT_WIFI_READY:
        LogManager::getInstance().info("WifiManager", "WiFi interface ready");
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        LogManager::getInstance().info("WifiManager", "WiFi scan completed");
        break;
    case SYSTEM_EVENT_STA_START:
        LogManager::getInstance().info("WifiManager", "Station mode started");
        break;
    case SYSTEM_EVENT_STA_STOP:
        LogManager::getInstance().info("WifiManager", "Station mode stopped");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Connected to access point: %s", WiFi.SSID());
        LogManager::getInstance().info("WifiManager", buffer);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        LogManager::getInstance().warning("WifiManager", "Connection lost. Trying to reconnect...");
        if (WifiManager::getInstance().isTargetSSIDFound(WIFI_SSID))
            WiFi.reconnect();
        else
        {
            LogManager::getInstance().error("WifiManager", "SSID not found retry in 5 Seconds");
            LogManager::getInstance().delay(5000);
        }
        break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    {
        LogManager::getInstance().info("WifiManager", "AP authentication mode changed");
        break;
    }
    case SYSTEM_EVENT_STA_GOT_IP:
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Obtained IP address: %s", WiFi.localIP().toString().c_str());
        LogManager::getInstance().info("WifiManager", buffer);
        break;
    }
    case SYSTEM_EVENT_STA_LOST_IP:
        LogManager::getInstance().warning("WifiManager", "Lost IP address");
        break;

    default:
        break;
    }
}