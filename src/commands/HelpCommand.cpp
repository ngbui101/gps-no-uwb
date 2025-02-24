#include "commands/HelpCommand.h"

bool HelpCommand::execute(const std::vector<String>& args, ICommandContext& context) {
    CommandManager& commandManager = CommandManager::getInstance();
    
    if (!args.empty()) {
        String commandName = args[0];
        if (commandManager.hasCommand(commandName)) {
            auto cmd = commandManager.getCommand(commandName);
            String response = "Help for command '" + commandName + "':\n";
            response += "  Description: " + String(cmd->getDescription()) + "\n";
            context.sendResponse(response.c_str());
            return true;
        } else {
            String response = "Unknown command '" + commandName + "'. Type 'help' for a list of commands.";
            context.sendResponse(response.c_str());
            return false;
        }
    }
    
    String response = "Available commands:\n";
    const auto& commands = commandManager.getCommands();
    for (const auto& cmd : commands) {
        response += "  " + cmd.first + " - " + cmd.second->getDescription() + "\n";
    }
    response += "\nType 'help <command>' for detailed information about a specific command.";
    
    context.sendResponse(response.c_str());
    return true;
}
