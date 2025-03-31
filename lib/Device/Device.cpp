#include "Device.h"

bool Device::begin()
{
    // Device
    dwt_setleds(DWT_LEDS_DISABLE); // disable annoys
    // Wifi
    if (!(wifiManager.begin() || wifiManager.connect()))
    {
        logManager.debug("WifiManager", "failed");
        return false;
    }
    wifiManager.update();
    // char ssidMessage[128];
    // char wifiRSSIMessage[128];
    // char ipMessage[128];
    // snprintf(ssidMessage, sizeof(ssidMessage), "Connected to %s", wifiManager.getSSID());
    // snprintf(wifiRSSIMessage, sizeof(wifiRSSIMessage), "RSSI: %d", wifiManager.getRSSI());
    // snprintf(wifiRSSIMessage, sizeof(ipMessage), "IP: %s", wifiManager.getIP());
    // logManager.info("WifiManager", ssidMessage);
    // logManager.info("WifiManager", wifiRSSIMessage);
    // logManager.info("WifiManager", ipMessage);

    return true;
}

// Beispielhafte Implementierung von run_tag()
// Diese Methode könnte als "Transmitter" agieren.
void Device::run_tag()
{
}
