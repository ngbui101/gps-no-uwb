#include "managers/CommandManager.h"

bool CommandManager::begin()
{
    log.debug("CommandManager", "Initializing command system...");

    // TODO: check for state, cli lock
    registerCommand(std::make_shared<HelpCommand>());
    registerCommand(std::make_shared<PingCommand>());
    registerCommand(std::make_shared<WifiCommand>());

    mqttContext = std::unique_ptr<MQTTCommandContext>(new MQTTCommandContext());

    String mqttCommandTopic = String(mqttManager.getDeviceTopic()) + "/console";
    mqttManager.subscribe(mqttCommandTopic.c_str(), [this](const char *topic, const uint8_t *payload, unsigned int length)
                          { handleMQTTCommand(topic, payload, length); }, false);

    return true;
}

void CommandManager::registerCommand(std::shared_ptr<ICommand> command)
{
    if (!command)
        return;

    String commandName = command->getName();
    commands[commandName] = command;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Registered command: %s", commandName.c_str());
    log.debug("CommandManager", buffer);
}

void CommandManager::handleMQTTCommand(const char *topic, const uint8_t *payload, unsigned int length)
{
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    String command(message);

    if (command.startsWith("{") && command.endsWith("}"))
    {
        // handleMQTTJsonCommand(command);
    }
    else
    {
        executeCommand(command, *mqttContext);
    }
}

bool CommandManager::executeCommand(const String &commandStr, ICommandContext &context)
{
    std::vector<String> commandArgs;
    String currentArg;
    bool isQuoted = false;

    for (size_t i = 0; i < commandStr.length(); i++)
    {
        char c = commandStr[i];

        if (c == ' ' && !isQuoted)
        {
            if (currentArg.length() > 0)
            {
                commandArgs.push_back(currentArg);
                currentArg = "";
            }
        }
        else if (c == '"')
        {
            if (isQuoted)
            {
                commandArgs.push_back(currentArg);
                currentArg = "";
                isQuoted = false;

                while (i + 1 < commandStr.length() && commandStr[i + 1] == ' ')
                {
                    i++;
                }
            }
            else
            {
                isQuoted = true;
                if (currentArg.length() > 0)
                {
                    commandArgs.push_back(currentArg);
                    currentArg = "";
                }
            }
        }
        else
        {
            currentArg += c;
        }
    }

    if (currentArg.length() > 0)
    {
        commandArgs.push_back(currentArg);
    }

    if (commandArgs.size() == 0)
    {
        log.warning("CommandManager", "No command provided");
        return false;
    }

    String commandName = commandArgs[0];
    commandArgs.erase(commandArgs.begin());

    auto commandIterator = commands.find(commandName);
    if (commandIterator == commands.end())
    {
        context.sendResponse("Unknown command. Type 'help' for available commands.");
        return false;
    }

    return commandIterator->second->execute(commandArgs, context);
}

const std::map<String, std::shared_ptr<ICommand>> &CommandManager::getCommands() const
{
    return commands;
}

bool CommandManager::hasCommand(const String &name) const
{
    return commands.find(name) != commands.end();
}

std::shared_ptr<ICommand> CommandManager::getCommand(const String &name) const
{
    auto commandIterator = commands.find(name);
    if (commandIterator == commands.end())
    {
        return nullptr;
    }

    return commandIterator->second;
}