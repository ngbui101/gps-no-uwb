#include "Initiator.h"

bool Initiator::begin()
{
    // Init UWB module
    uwbManager.begin();
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

void Initiator::runTag()
{
    mqttManager.update();
    static unsigned long lastPublishTime = 0;
    unsigned long now = millis();
    // Publish a test message every 1 seconds
    if (now - lastPublishTime > 1000)
    {
        double distanz[NUM_DEVS - 1];
        uwbManager.initiator(&distanz[0]);
        lastPublishTime = now;
        char message[64];
        snprintf(message, sizeof(message), "Distanz %f", distanz[0]);
        mqttManager.publish("test", message, false, false);
    }
}
