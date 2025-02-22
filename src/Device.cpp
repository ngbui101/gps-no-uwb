#include "Device.h"

void Device::changeState(DeviceState& newState) {
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