#include "WifiManager.h"

bool WifiManager::begin()
{
    if (strlen(WIFI_SSID) == 0)
    {
        log.warning("WifiManager", "No SSID available");
        return false;
    }
    else if (strlen(WIFI_PASSWORD) == 0)
    {
        log.warning("WifiManager", "No Password available");
        return false;
    }
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (!WiFi.begin(WIFI_SSID, WIFI_PASSWORD))
    {
        char errorBuffer[256];
        snprintf(errorBuffer, sizeof(errorBuffer), "Fail to begin Wifi %s", WIFI_SSID);
        log.error("WifiManager", errorBuffer);
        return false;
    }

    long time_out = 20000; // 20s timeout
    long start_time = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if ((millis() - start_time) > time_out)
        {
            Serial.println();
            char errorBuffer[256];
            snprintf(errorBuffer, sizeof(errorBuffer), "Connect to %s fail, please check your ssid or pwd", WIFI_SSID);
            log.error("WifiManager", errorBuffer);
            return false;
        }
    }
    Serial.println();

    char ssidBuffer[128];
    char rssiBuffer[128];
    char ipBuffer[128];
    snprintf(ssidBuffer, sizeof(ssidBuffer), "Connect success to Wlan %s", WIFI_SSID);
    snprintf(rssiBuffer, sizeof(rssiBuffer), "RSSI: %d", WiFi.RSSI());
    snprintf(ipBuffer, sizeof(ipBuffer), "IP-Adress %s", WiFi.localIP().toString().c_str());

    log.debug("WifiManager", ssidBuffer);
    log.debug("WifiManager", rssiBuffer);
    log.debug("WifiManager", ipBuffer);

    return true;
}
