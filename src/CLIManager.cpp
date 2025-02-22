#include "CLIManager.h"

void CLIManager::registerCommand(std::shared_ptr<Command> command) {
    commands[command->getName()] = command;
}

void CLIManager::begin() {
    String deviceInputTopic = String(mqttManager.getDeviceTopic()) + String("/cli");
}