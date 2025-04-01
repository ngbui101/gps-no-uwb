#include "Device.h"

bool Device::begin()
{
    // Device

    // Wifi
    if (!(wifiManager.begin() && wifiManager.connect()))
    {
        logManager.error("WifiManager", "Failed to initialize Wifi, check for ssid&pwd and restart");
        return false;
    }
    if (!(mqttManager.begin() && mqttManager.connect()))
    {
        logManager.error("MqttManager", "Failed to initialize MQTT. Check the connection and restart");
        return false;
    }

    mqttManager.publish("/test", "Hello from esp32");
    return true;
}

// Beispielhafte Implementierung von run_tag()
// Diese Methode könnte als "Transmitter" agieren.
void Device::run_tag()
{
}
