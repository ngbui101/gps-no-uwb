#include "Initiator.h"

static long lastPublishTime = -9999;

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
    // // Initialize MQTT
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

    unsigned long now = millis();
    char buffer[1024] = "";
    uwbManager.initiator(buffer);
    if ((now - lastPublishTime > 1000) && (buffer[0] != '\0'))
    {
        Serial.println(buffer);
        mqttManager.publish("test", buffer, false, false);
        lastPublishTime = millis();
    }
}
