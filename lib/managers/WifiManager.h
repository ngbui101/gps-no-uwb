#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "LogManager.h"
#include "ConfigDefines.h"

enum class WiFiStatus
{
    UNINITIALIZED,
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    CONNECTION_FAILED,
    WRONG_PASSWORD,
    NO_SSID_AVAILABLE,
    __DELIMITER__
};

class WifiManager
{
private:
    WifiManager()
        : log(LogManager::getInstance()) {}
    LogManager &log;

public:
    WifiManager(const WifiManager &) = delete;
    static WifiManager &getInstance()
    {
        static WifiManager instance;
        return instance;
    }

    /**
     * @brief Initialisiert das WLAN und stellt eine Verbindung zum konfigurierten Netzwerk her.
     *
     * Diese Methode überprüft zunächst, ob sowohl die SSID als auch das Passwort vorhanden sind.
     * Ist eine der beiden Angaben leer, wird ein Warn-Log ausgegeben und die Initialisierung mit false abgebrochen.
     *
     * Anschließend wird der WiFi-Modus auf Station (WIFI_STA) gesetzt und eine bestehende Verbindung getrennt.
     * Mit WiFi.begin() wird versucht, eine Verbindung zum WLAN herzustellen.
     * Falls WiFi.begin() fehlschlägt, wird ein Fehler geloggt und die Methode gibt false zurück.
     *
     * Danach wird in einer Schleife gewartet, bis der Verbindungsstatus WL_CONNECTED erreicht wird.
     * Es wird ein Timeout von 20 Sekunden definiert. Überschreitet die Wartezeit diesen Wert,
     * wird die Verbindung als fehlgeschlagen betrachtet, ein Fehler geloggt und false zurückgegeben.
     *
     * Bei erfolgreicher Verbindung werden Debug-Logs ausgegeben, die unter anderem die verbundene SSID,
     * den RSSI-Wert (Signalstärke) und die erhaltene IP-Adresse (als C-String) enthalten.
     *
     * @return true, wenn das WLAN erfolgreich verbunden wurde, sonst false.
     */
    bool begin();
};

#endif