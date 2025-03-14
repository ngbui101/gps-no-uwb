#include "managers/BluetoothManager.h"

void BluetoothManager::ServerCallbacks::onConnect(BLEServer *pServer)
{
    manager.status = BluetoothStatus::ADVERTISING;
    manager.updatePayload();
    manager.log.info("BluetoothManager", "Device is advertising");
}

void BluetoothManager::ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    manager.status = BluetoothStatus::DISCONNECTED;
    manager.updatePayload();
    manager.log.info("BluetoothManager", "Device is no longer advertising");
}

void BluetoothManager::ClientCallbacks::onConnect(BLEClient *pClient)
{
    manager.status = BluetoothStatus::CONNECTED;
    manager.updatePayload();
    manager.log.info("BluetoothManager", "Connected to device");
}

void BluetoothManager::ClientCallbacks::onDisconnect(BLEClient *pClient)
{
    manager.status = BluetoothStatus::DISCONNECTED;
    manager.updatePayload();
    manager.log.info("BluetoothManager", "Disconnected from device");
}

bool BluetoothManager::begin()
{
    if (status != BluetoothStatus::UNINITIALIZED)
    {
        return true;
    }
    log.debug("BluetoothManager", "Initializing Bluetooth...");

    const RuntimeConfig &config = configManager.getRuntimeConfig();
    BLEDevice::init(config.device.name);
    status = BluetoothStatus::INITIALIZED;

    return true;
}

bool BluetoothManager::setMode(BluetoothMode mode)
{
    if (mode == this->mode)
    {
        return true;
    }

    cleanup();

    this->mode = mode;
    status = BluetoothStatus::UNINITIALIZED;

    switch (mode)
    {
    case BluetoothMode::SERVER:
        return initServer();
    case BluetoothMode::CLIENT:
        return initClient();
    case BluetoothMode::NONE:
        log.info("BluetoothManager", "Bluetooth is disabled");
        return true;
    default:
        log.error("BluetoothManager", "Invalid mode");
        return false;
    }
}

void BluetoothManager::cleanup()
{
    disconnect();

    if (pServer)
    {
        pServer->getAdvertising()->stop();
        delete pServer;
        pServer = nullptr;
    }

    if (pClient)
    {
        delete pClient;
        pClient = nullptr;
    }

    if (pCharacteristic)
    {
        delete pCharacteristic;
        pCharacteristic = nullptr;
    }

    status = BluetoothStatus::INITIALIZED;
    mode = BluetoothMode::NONE;
}

bool BluetoothManager::initServer()
{
    log.info("BluetoothManager", "Initializing server");
    RuntimeConfig &config = configManager.getRuntimeConfig();

    pServer = BLEDevice::createServer();
    if (!pServer)
    {
        log.error("BluetoothManager", "Failed to create server");
        return false;
    }

    pServer->setCallbacks(new ServerCallbacks(*this));

    BLEService *pDeviceInfoService = pServer->createService((uint16_t)0x180A);
    if (!pDeviceInfoService)
    {
        log.error("BluetoothManager", "Failed to create device info service");
        return false;
    }

    BLEService *pCustomService = pServer->createService(
        configManager.getRuntimeConfig().bluetooth.serviceUUID);
    if (!pCustomService)
    {
        log.error("BluetoothManager", "Failed to create custom service");
        return false;
    }

    pCharacteristic = pCustomService->createCharacteristic(
        config.bluetooth.charUUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    if (!pCharacteristic)
    {
        log.error("BluetoothManager", "Failed to create characteristic");
        return false;
    }

    pCharacteristic->addDescriptor(new BLE2902());

    pDeviceInfoService->start();
    pCustomService->start();

    pServer->getAdvertising()->start();
    status = BluetoothStatus::ADVERTISING;

    log.info("BluetoothManager", "Server initialized");
    return true;
}

bool BluetoothManager::initClient()
{
    log.info("BluetoothManager", "Initializing client");

    pClient = BLEDevice::createClient();
    if (!pClient)
    {
        log.error("BluetoothManager", "Failed to create client");
        return false;
    }

    pClient->setClientCallbacks(new ClientCallbacks(*this));
    status = BluetoothStatus::SCANNING;

    log.info("BluetoothManager", "Client initialized");
    return true;
}

bool BluetoothManager::connect(const char *target)
{
    if (mode != BluetoothMode::CLIENT)
    {
        log.error("BluetoothManager", "Cannot connect in server mode");
        return false;
    }

    const RuntimeConfig &config = configManager.getRuntimeConfig();

    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Scanning for device: %s", target);
    log.info("BluetoothManager", buffer);

    BLEScanResults results = pBLEScan->start(config.bluetooth.timeout);

    for (int i = 0; i < results.getCount(); i++)
    {
        BLEAdvertisedDevice advertisedDevice = results.getDevice(i);
        if (advertisedDevice.getName() == target)
        {
            if (pClient->connect(&advertisedDevice))
            {
                log.info("BluetoothManager", "Connection successful");
                return true;
            }
            status = BluetoothStatus::CONNECTION_FAILED;
            log.error("BluetoothManager", "Connection failed");
            return false;
        }
    }

    log.error("BluetoothManager", "Device not found");
    return false;
}

void BluetoothManager::disconnect()
{
    if (mode == BluetoothMode::SERVER && pServer)
    {
        pServer->getAdvertising()->stop();
    }
    else if (mode == BluetoothMode::CLIENT && pClient)
    {
        pClient->disconnect();
    }

    status = BluetoothStatus::DISCONNECTED;
    log.info("BluetoothManager", "Disconnected");
}

void BluetoothManager::updatePayload()
{
    if (!pCharacteristic)
    {
        return;
    }

    if (mode == BluetoothMode::SERVER && status != BluetoothStatus::ADVERTISING)
    {
        return;
    }

    if (mode == BluetoothMode::CLIENT && status != BluetoothStatus::CONNECTED)
    {
        return;
    }

    const RuntimeConfig &config = configManager.getRuntimeConfig();

    DynamicJsonDocument doc(256);
    doc["device_id"] = config.device.name;
    doc["mac"] = config.device.macAddress;
    doc["chip_id"] = String(config.device.chipID);

    String payload;
    serializeJson(doc, payload);

    pCharacteristic->setValue(payload.c_str());
    pCharacteristic->notify();
}

void BluetoothManager::update()
{
    if (status == BluetoothStatus::ADVERTISING && mode == BluetoothMode::SERVER)
    {
        updatePayload();
    }
}