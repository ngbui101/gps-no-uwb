#include "Device.h"

bool Device::begin()
{
    // Initialize and connect WiFi
    if (!(wifiManager.begin() && wifiManager.connect()))
    {
        logManager.error("WifiManager", "Failed to initialize WiFi, check for SSID & password and restart");
        return false;
    }

    // Initialize MQTT
    if (!(mqttManager.begin()))
    {
        logManager.error("MQTTManager", "Failed to initialize MQTT, check the connection and restart");
        return false;
    }
    return true;
}

void Device::run_tag()
{
    mqttManager.update();
    static unsigned long lastPublishTime = 0;
    static unsigned int counter = 0;
    unsigned long now = millis();
    // Publish a test message every 5 seconds
    if (now - lastPublishTime > 5000)
    {
        lastPublishTime = now;
        char message[64];
        snprintf(message, sizeof(message), "Hello from esp32, count: %u", counter++);
        mqttManager.publish("test", message, false, false);
    }
}
