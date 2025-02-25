#ifndef I_EXTENDED_COMMAND_H
#define I_EXTENDED_COMMAND_H

#include <map>
#include "ICommand.h"

class ExtendedCommand : public ICommand {
protected:
    using SubCommandHandler = std::function<bool(const std::vector<String>&, ICommandContext&)>;
    std::map<String, SubCommandHandler> subCommands;
    std::map<String, String> subCommandDescriptions;

    bool executeSubCommand(const std::vector<String>& args, ICommandContext& context) {
        if(args.empty()) {
            return executeDefaultSubCommand(context);
        }

        String subCommand = args[0];
        std::vector<String> subArgs(args.begin() + 1, args.end());

        auto command = subCommands.find(subCommand);
        if(command == subCommands.end()) {
            context.sendResponse("Invalid subcommand");
            String response = "Unknown subcommand: " + subCommand + "\nAvailable subcommands:";
            for (const auto& cmd : subCommands) {
                response += "\n  " + cmd.first + " - " + getSubCommandDescription(cmd.first);
            }

            context.sendResponse(response.c_str());
            return false;
        }
        
        return command->second(subArgs, context);
    }

    virtual bool executeDefaultSubCommand(ICommandContext& context) {
        String response = "Please provide a subcommand\nAvailable subcommands:";
        for (const auto& cmd : subCommands) {
            response += "\n  " + cmd.first + " - " + getSubCommandDescription(cmd.first);
        }

        context.sendResponse(response.c_str());
        return true;
    }

public:
    bool hasSubCommands() const override { return !subCommands.empty(); }

    std::vector<String> getSubCommands() const override {
        std::vector<String> result;
        for(const auto& cmd : subCommands) {
            result.push_back(cmd.first);
        }

        return result;
    }
    
    const char* getSubCommandDescription(const String& subCommand) const override {
        auto descriptionIterator = subCommandDescriptions.find(subCommand);
        if(descriptionIterator == subCommandDescriptions.end()) {
            return "";
        }

        return descriptionIterator->second.c_str();
    }
    
    bool execute(const std::vector<String>& args, ICommandContext& context) override {
        return executeSubCommand(args, context);
    }
};

#endif