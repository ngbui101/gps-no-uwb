#include "Device.h"

bool Device::begin(){
    log.debug("Device", "Initializing Device...");

    if(!configManager.begin()) {
        log.error("Device", "Failed to initialize ConfigManager");
        false;
    }

    if (configManager.hasConfigDefinesChanged()) {
        log.info("Device", "ConfigDefines has changed, updating device config");
        configManager.updateDeviceConfig();
    }

    if (!commandManager.begin()) {
        log.error("Device", "Failed to initialize CommandManager");
        return false;
    }

    registerCommands();

    return true;
}

void Device::registerCommands(){
    commandManager.registerCommand(std::make_shared<HelpCommand>());
    commandManager.registerCommand(std::make_shared<PingCommand>());
}

void Device::changeState(IDeviceState& newState) {
    if (currentState) {
        currentState->exit();
    }

    currentState = &newState;
    
    if(currentState){
        currentState->enter();
    }
}

void Device::update() {
    if (!currentState) {
       return;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    currentState->update();

    handleSerialCommands();
    updateDeviceStatus();
}

void Device::handleSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.length() > 0) {
            commandManager.executeCommand(command, serialContext);
        }
    }
}

void Device::updateDeviceStatus() {
    RuntimeConfig& config = configManager.getRuntimeConfig();
    uint32_t now = millis();

    if (now - lastStatusUpdate >= config.device.statusUpdateInterval) {
        sendDeviceStatus();
        lastStatusUpdate = now;
    }
}

void Device::sendDeviceStatus(){
    StaticJsonDocument<JSON_DOC_SIZE> doc;
    
    doc["status"] = "online";
    doc["uptime"] = millis();
    doc["rssi"] = WiFi.RSSI();
    doc["state"] = currentState ? currentState->getStateIdentifierString() : "UNKNOWN";
    
    JsonObject heap = doc.createNestedObject("heap");
    heap["free"] = ESP.getFreeHeap();
    heap["min_free"] = ESP.getMinFreeHeap();
    heap["max_alloc"] = ESP.getMaxAllocHeap();

    String payload;
    serializeJson(doc, payload);

    log.debug("Device", payload.c_str());

    if (mqttManager.isConnected()) {
        RuntimeConfig& config = configManager.getRuntimeConfig();
        mqttManager.publish("status", payload.c_str(), true);
    }
}

const char* Device::getDeviceStatusString(DeviceStatus status) {
    switch(status) {
        case DeviceStatus::BOOTING: return "BOOTING";
        case DeviceStatus::IDLE: return "IDLE";
        case DeviceStatus::SETUP: return "SETUP";
        case DeviceStatus::TRANSITIONING: return "TRANSITIONING";
        case DeviceStatus::ACTION: return "ACTION";
        case DeviceStatus::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}