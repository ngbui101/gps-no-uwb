#include "commands/HelpCommand.h"

bool HelpCommand::execute(const std::vector<String>& args, ICommandContext& context) {
    CommandManager& commandManager = CommandManager::getInstance();
    
    if (!args.empty()) {
        String commandName = args[0];
        if (commandManager.hasCommand(commandName)) {
            auto cmd = commandManager.getCommand(commandName);
            String response = "Help for command '" + commandName + "':\n";
            response += "  Description: " + String(cmd->getDescription()) + "\n";
            
            if (cmd->hasSubCommands()) {
                response += "\nSubcommands:\n";
                std::vector<String> subCommands = cmd->getSubCommands();
                for (const auto& subCmd : subCommands) {
                    response += "  " + subCmd + " - " + String(cmd->getSubCommandDescription(subCmd)) + "\n";
                    
                    // Parameter anzeigen
                    std::vector<CommandParameter> params = cmd->getSubCommandParameters(subCmd);
                    if (!params.empty()) {
                        response += "    Parameters:\n";
                        for (const auto& param : params) {
                            response += "      " + param.name + ": " + param.description;
                            if (param.required) {
                                response += " (required)";
                            } else if (param.defaultValue.length() > 0) {
                                response += " (default: " + param.defaultValue + ")";
                            }
                            response += "\n";
                        }
                    }
                }
            }
            
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