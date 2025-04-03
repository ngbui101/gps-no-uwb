#include "Initiator.h"

float distances[NUM_NODES - 1];

bool Initiator::begin()
{
    // Init UWB module
    uwbManager.begin();
    // Initialize and connect WiFi
    // if (!(wifiManager.begin() && wifiManager.connect()))
    // {
    //     logManager.error("WifiManager", "Failed to initialize WiFi, check for SSID & password and restart");
    //     return false;
    // }

    // // Initialize MQTT
    // if (!(mqttManager.begin()))
    // {
    //     logManager.error("MQTTManager", "Failed to initialize MQTT, check the connection and restart");
    //     return false;
    // }
    return true;
}

void Initiator::runTag()
{
    // mqttManager.update();
    // static unsigned long lastPublishTime = 0;
    // unsigned long now = millis();

    // if (now - lastPublishTime > 200)
    // {
        uwbManager.initiator(distances);

        // for (int i = 0; i < NUM_NODES - 1; i++)
        // {
        //     if (distances[i] > 0.01f)
        //     {
                // char buffer[255];

                // snprintf(buffer, sizeof(buffer), "Distance to %i: %.3fm", i, distances[i]);
                // logManager.info("Initiator", buffer);
                
                // mqttManager.publish("test", buffer, false, false);
                
        //         distances[i] = 0.0f;
        //     }
        // }

        // lastPublishTime = now;
        // }
}
