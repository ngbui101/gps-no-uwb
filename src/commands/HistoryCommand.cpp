#include "commands/HistoryCommand.h"
#include "managers/CommandManager.h"

HistoryCommand::HistoryCommand() : commandManager(CommandManager::getInstance())
{
    subCommands["show"] = [this](const std::vector<String> &args, ICommandContext &context)
    {
        return showCommandHistoryCmd(args, context);
    };
    subCommandDescriptions["show"] = "Shows the command history";

    subCommands["clear"] = [this](const std::vector<String> &args, ICommandContext &context)
    {
        commandManager.clearCommandHistory();

        context.sendResponse("Command history cleared");
        return true;
    };
    subCommandDescriptions["clear"] = "Clears the command history";
}

bool HistoryCommand::showCommandHistoryCmd(const std::vector<String> &args, ICommandContext &context)
{
    auto commandHistory = commandManager.getCommandHistory();
    size_t commandHistoryIndex = commandManager.getCommandHistoryIndex();

    char buffer[COMMAND_LINE_LENGTH * COMMAND_HISTORY_SIZE + 16];
    size_t bufferPos = 0;

    for (int i = 0; i < COMMAND_HISTORY_SIZE; i++)
    {
        if (commandHistory[i][0] != '\0')
        {
            int lineLength = snprintf(buffer + bufferPos, sizeof(buffer) - bufferPos,
                                      "%2d: %s\n",
                                      commandHistoryIndex - (COMMAND_HISTORY_SIZE - i), commandHistory[i]);

            strcpy(buffer + bufferPos, commandHistory[i]);
            bufferPos += lineLength;
        }
    }

    context.sendResponse(buffer);

    return true;
}

bool HistoryCommand::execute(const std::vector<String> &args, ICommandContext &context)
{
    if (args.empty())
    {
        return showCommandHistoryCmd(args, context);
    }

    return executeSubCommand(args, context);
}