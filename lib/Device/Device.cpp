#include "Device.h"

bool Device::begin()
{
    // Device

    // Wifi
    if (!(wifiManager.begin()))
    {
        logManager.debug("WifiManager", "Failed");
        return false;
    }

    return true;
}

// Beispielhafte Implementierung von run_tag()
// Diese Methode könnte als "Transmitter" agieren.
void Device::run_tag()
{
}
