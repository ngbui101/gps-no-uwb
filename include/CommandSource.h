#ifndef COMMAND_SOURCE_H
#define COMMAND_SOURCE_H

#include "MQTTManager.h"

class CommandSource {
public:
    virtual ~CommandSource() = default;
    virtual void sendResponse(const char* response) = 0;
};

class SerialCommandSource : public CommandSource {
public:
    void sendResponse(const char* response) override;
};

class MQTTCommandSource : public CommandSource {
private:
    MQTTManager& mqttManager;
    char topic[128];

public:
    MQTTCommandSource() : 
        mqttManager(MQTTManager::getInstance()) {
            snprintf(topic, sizeof(topic), "%s/cli", mqttManager.getDeviceTopic());
        }


    void sendResponse(const char* response) override;
};

#endif