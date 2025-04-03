#include "Initiator.h"

float distances[NUM_NODES - 1];

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

    // Alle 200ms einen neuen Payload erstellen und veröffentlichen
    if (now - lastPublishTime > 200)
    {
        // Nehme an, dass distances ein globales oder Mitgliedsarray vom Typ float ist,
        // z. B. float distances[NUM_NODES - 1];
        uwbManager.initiator(distances);

        // JSON-Payload als String zusammenbauen
        // char payload[255] = "";

        // Durchlaufe alle Elemente im distances-Array
        for (int i = 0; i < NUM_NODES - 1; i++)
        {
            if (distances[i] > 0.01f)
            {
                // char buffer[255];

                // // Erstelle einen JSON-Objekt-String für den aktuellen Knoten
                // snprintf(buffer, sizeof(buffer), "Distance to %i: %.3fm", i, distances[i]);
                // logManager.info("Initiator", buffer);
                // // Sende den Payload per MQTT
                // mqttManager.publish("test", buffer, false, false);
                // Setze den Wert zurück, falls gewünscht
                distances[i] = 0.0f;
            }
        }

        lastPublishTime = now;

        // Logge den Payload (optional)
        }
}
