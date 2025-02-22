#ifndef CLI_MANAGER_H
#define CLI_MANAGER_H

#include <map>
#include <memory>
#include "Command.h"
#include "MQTTManager.h"
#include "ConfigManager.h"
#include "CommandSource.h"
#include "CommandInterpreter.h"


class CLIManager {
private:
    CLIManager()
        : mqttManager(MQTTManager::getInstance()) {}

    MQTTManager& mqttManager;

    std::map<String, std::shared_ptr<Command>> commands;
    CommandInterpreter commandInterpreter;
    SerialCommandSource serialCommandSource;
    MQTTCommandSource mqttCommandSource;
    String serialBuffer;

    void handleMQTTCommand(const char* topic, const uint8_t* payload, unsigned int length);

public:
    CLIManager(CLIManager const&) = delete;
    void operator=(CLIManager const&) = delete;

    static CLIManager& getInstance() {
        static CLIManager instance;
        return instance;
    }

    void registerCommand(std::shared_ptr<Command> command);
    void begin();
    void update();

};

#endif