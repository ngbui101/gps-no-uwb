#include "Device.h"

bool Device::begin()
{
    // Device

    // Wifi
    if (!(wifiManager.begin() && wifiManager.connect()))
    {
        logManager.debug("WifiManager", "Failed");
        return false;
    }
    if (!(mqttManager.begin() && mqttManager.connect()))
    {
        logManager.debug("WifiManager", "Failed");
        return false;
    }
    mqttManager.publish("/test", "Hello World");
    return true;
}

// Beispielhafte Implementierung von run_tag()
// Diese Methode könnte als "Transmitter" agieren.
void Device::run_tag()
{
}
