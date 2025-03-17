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
}

bool HistoryCommand::showCommandHistoryCmd(const std::vector<String> &args, ICommandContext &context)
{
    auto commandHistory = commandManager.getCommandHistory();
    size_t commandHistoryIndex = commandManager.getCommandHistoryIndex();

    char buffer[COMMAND_LINE_LENGTH * COMMAND_HISTORY_SIZE + COMMAND_HISTORY_SIZE];

    char tempLine[COMMAND_LINE_LENGTH + 8]; // Temporärer Puffer für jede Zeile
    size_t bufferPos = 0;                   // Position im Puffer

    // Kopfzeile hinzufügen
    bufferPos += snprintf(buffer + bufferPos, sizeof(buffer) - bufferPos, "Command History:\n");

    for (int i = 0; i < COMMAND_HISTORY_SIZE; i++)
    {
        if (commandHistory[i][0] != '\0')
        {
            // Formatiere die Zeile mit Nummer und Befehl
            int lineLength = snprintf(tempLine, sizeof(tempLine),
                                      "%2d: %s\n",
                                      commandHistoryIndex - (COMMAND_HISTORY_SIZE - i), commandHistory[i]);

            // Zur Hauptausgabe hinzufügen, wenn genug Platz ist
            if (bufferPos + lineLength < sizeof(buffer))
            {
                strcpy(buffer + bufferPos, tempLine);
                bufferPos += lineLength;
            }
            else
            {
                // Puffer würde überlaufen, abbrechen
                bufferPos += snprintf(buffer + bufferPos, sizeof(buffer) - bufferPos,
                                      "... (output truncated)\n");
                break;
            }
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