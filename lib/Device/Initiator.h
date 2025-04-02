#ifndef Initiator_H
#define Initiator_H

#include "LogManager.h"
#include "MQTTManager.h"
#include "WifiManager.h"
#include "UWBManager.h"

/**
 * @brief Die Klasse Initiator koordiniert den Betrieb des Geräts als Initiator.
 *
 * Diese Singleton-Klasse verwaltet und stellt den Zugriff auf die
 * MQTT-, Log-, WLAN- und UWB-Manager bereit. Sie bietet Methoden zur
 * Initialisierung des Systems und zur Ausführung der Transmitter-Routine.
 */
class Initiator
{
private:
    /**
     * @brief Privater Konstruktor (Singleton).
     *
     * Erzeugt eine Instanz, die die Referenzen auf die Manager speichert.
     *
     * @param mqttMgr Referenz auf den MQTTManager.
     * @param logMgr Referenz auf den LogManager.
     * @param wifiMgr Referenz auf den WifiManager.
     * @param uwbMgr Referenz auf den UWBManager.
     */
    Initiator(MQTTManager &mqttMgr, LogManager &logMgr, WifiManager &wifiMgr, UWBManager &uwbMgr)
        : mqttManager(mqttMgr), logManager(logMgr), wifiManager(wifiMgr), uwbManager(uwbMgr)
    {
    }

    MQTTManager &mqttManager;
    LogManager &logManager;
    WifiManager &wifiManager;
    UWBManager &uwbManager;

public:
    Initiator(const Initiator &) = delete;
    Initiator &operator=(const Initiator &) = delete;
    /**
     * @brief Gibt die Singleton-Instanz des Initiators zurück.
     *
     * Diese Methode liefert eine Referenz auf die einzige Instanz der Klasse.
     *
     * @return Referenz auf die Initiator-Instanz.
     */
    static Initiator &getInstance()
    {
        static Initiator instance(MQTTManager::getInstance(),
                                  LogManager::getInstance(),
                                  WifiManager::getInstance(),
                                  UWBManager::getInstance());
        return instance;
    }

    MQTTManager &getMQTTManager() { return mqttManager; }
    LogManager &getLogManager() { return logManager; }
    WifiManager &getWifiManager() { return wifiManager; }
    UWBManager &getUWBManager() { return uwbManager; }
    /**
     * @brief Initialisiert alle Systemkomponenten.
     *
     * Führt notwendige Initialisierungen (z. B. MQTT, WLAN, UWB) durch.
     *
     * @return true, wenn die Initialisierung erfolgreich war, andernfalls false.
     */
    bool begin();

    /**
     * @brief Führt die Transmitter-Routine aus.
     *
     * Diese Methode implementiert die Funktionalität des Initiators als
     * Transmitter (Tag-Modus).
     */
    void runTag();
};

#endif // Initiator_H
