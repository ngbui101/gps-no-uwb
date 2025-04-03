#include "Responder.h"

bool Responder::begin()
{
    // Initialisiere das UWB-Modul
    uwbManager.begin();

    // Initialisiere und verbinde WLAN
    // if (!(wifiManager.begin() && wifiManager.connect()))
    // {
    //     logManager.error("WifiManager", "Failed to initialize WiFi, check for SSID & password and restart");
    //     return false;
    // }

    // Initialisiere MQTT
    // if (!(mqttManager.begin()))
    // {
    //     logManager.error("MQTTManager", "Failed to initialize MQTT, check the connection and restart");
    //     return false;
    // }
    return true;
}

void Responder::runAnchor()
{
    // MQTT-Update verarbeiten
    // mqttManager.update();

    // Führe die Responder-Routine aus, die eingehende UWB-Nachrichten verarbeitet und ggf. Antworten sendet
    uwbManager.responder();
}
