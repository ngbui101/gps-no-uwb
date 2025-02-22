#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <ArduinoJson.h>
#include "CommandSource.h"

class CommandInterpreter {
private:
    bool isJsonCommand(const char* command);
    bool parseJson(const char* input, String& command, String& subcommand, JsonObject& params);
    bool parseCliStyle(const char* input, String& command, String& subcommand, JsonObject& params);

public:
    bool interpret(const char* input, CommandSource* source);
};

#endif