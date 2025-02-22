#include "CommandSource.h"

void SerialCommandSource::sendResponse(const char* response) {
    Serial.println(response);
}

void MQTTCommandSource::sendResponse(const char* response) {
    mqttManager.publish(topic, response);
}