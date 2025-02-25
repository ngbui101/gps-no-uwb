#include "managers/CommandManager.h"

bool CommandManager::begin() {
    log.debug("CommandManager", "Initializing command system...");

    //TODO: check for state, cli lock

    return true;
}

void CommandManager::registerCommand(std::shared_ptr<ICommand> command){
    if(!command) return;

    String commandName = command->getName();
    commands[commandName] = command;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Registered command: %s", commandName.c_str());
    log.debug("CommandManager", buffer);
}

bool CommandManager::executeCommand(const String& commandStr, ICommandContext& context){
    std::vector<String> commandArgs;
    String currentArg;
    bool isQuoted = false;

    for(char c : commandStr) {
        if(c == ' ' && !isQuoted) {
            if(currentArg.length() > 0) {
                commandArgs.push_back(currentArg);
                currentArg = "";
            }
        } else if(c == '"') {
            isQuoted = !isQuoted;
        } else {
            currentArg += c;
        }
    }

    if(currentArg.length() > 0) {
        commandArgs.push_back(currentArg);
    }

    if (commandArgs.size() == 0) {
        log.warning("CommandManager", "No command provided");
        return false;
    }

    String commandName = commandArgs[0];
    commandArgs.erase(commandArgs.begin());

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Executing command: %s", commandName.c_str());
    log.debug("CommandManager", buffer);

    auto commandIterator = commands.find(commandName);
    if (commandIterator == commands.end()) {
        context.sendResponse("Unknown command. Type 'help' for available commands.");
        return false;
    }

    return commandIterator->second->execute(commandArgs, context);
}

const std::map<String, std::shared_ptr<ICommand>>& CommandManager::getCommands() const {
    return commands;
}

bool CommandManager::hasCommand(const String& name) const {
    return commands.find(name) != commands.end();
}

std::shared_ptr<ICommand> CommandManager::getCommand(const String& name) const {
    auto commandIterator = commands.find(name);
    if (commandIterator == commands.end()) {
        return nullptr;
    }

    return commandIterator->second;
}