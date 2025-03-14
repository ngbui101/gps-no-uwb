#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#define BLE_DEVICE_INFO_SERVICE_UUID "180A"
#define BLE_DEVICE_INFO_CHARACTERISTIC_UUID "2A23"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include "ConfigManager.h"
#include "LogManager.h"

enum class BluetoothMode
{
    NONE,
    SERVER,
    CLIENT,
    __DELIMITER__
};

enum class BluetoothStatus
{
    UNINITIALIZED,
    INITIALIZED,
    ADVERTISING,
    SCANNING,
    CONNECTED,
    DISCONNECTED,
    CONNECTION_FAILED,
    __DELIMITER__
};

class BluetoothManager
{
private:
    BluetoothManager()
        : status(BluetoothStatus::UNINITIALIZED), mode(BluetoothMode::NONE), pServer(nullptr), pClient(nullptr), pCharacteristic(nullptr), configManager(ConfigManager::getInstance()), log(LogManager::getInstance()) {}

    ConfigManager &configManager;
    LogManager &log;

    BluetoothStatus status;
    BluetoothMode mode;
    BLEServer *pServer;
    BLEClient *pClient;
    BLECharacteristic *pCharacteristic;

    bool initServer();
    bool initClient();
    void updatePayload();
    void cleanup();

    const char *getBluetoothStatusString(BluetoothStatus status);
    constexpr size_t getBluetoothStatusCount() { return static_cast<size_t>(BluetoothStatus::__DELIMITER__); };

    class ServerCallbacks : public BLEServerCallbacks
    {
    private:
        BluetoothManager &manager;

    public:
        ServerCallbacks(BluetoothManager &manager)
            : manager(manager) {}

        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };

    class ClientCallbacks : public BLEClientCallbacks
    {
    private:
        BluetoothManager &manager;

    public:
        ClientCallbacks(BluetoothManager &manager) : manager(manager) {}
        void onConnect(BLEClient *pClient) override;
        void onDisconnect(BLEClient *pClient) override;
    };

public:
    BluetoothManager(const BluetoothManager &) = delete;
    void operator=(const BluetoothManager &) = delete;

    static BluetoothManager &getInstance()
    {
        static BluetoothManager instance;
        return instance;
    }

    bool begin();
    void update();
    bool connect(const char *target);
    void disconnect();

    bool setMode(BluetoothMode mode);

    const char *getStatusString();
    const char *getModeString();
};

#endif