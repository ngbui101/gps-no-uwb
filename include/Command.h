#ifndef COMMAND_H
#define COMMAND_H

#include <ArduinoJson.h>
#include "CommandSource.h"

class Command {
public:
    virtual ~Command() = default;
    virtual const char* getName() = 0;
    virtual const char* getDescription() = 0;
    virtual bool execute(const String& subcommand, const JsonObject& parameters, CommandSource* source) = 0;
    virtual void printHelp(CommandSource* source) = 0;

};

#endif