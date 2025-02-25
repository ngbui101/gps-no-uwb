#ifndef MQTT_COMMAND_CONTEXT_H
#define MQTT_COMMAND_CONTEXT_H

#include "managers/MQTTManager.h"
#include "interfaces/ICommandContext.h"

class MQTTCommandContext : public ICommandContext {
public:
    MQTTCommandContext()
        : mqttManager(MQTTManager::getInstance()) {
            Serial.println(mqttManager.getDeviceTopic());
            responseTopic = String(mqttManager.getDeviceTopic()) + "/response";
        }

    void sendResponse(const char* response) {
        mqttManager.publish(responseTopic.c_str(), response);
    }

    void setResponseTopic(const char* topic) {
        responseTopic = topic;
    }

private:
    MQTTManager& mqttManager;
    String responseTopic;

};

#endif