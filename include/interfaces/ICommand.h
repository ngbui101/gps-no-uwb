#ifndef I_COMMAND_H
#define I_COMMAND_H

#include <ArduinoJson.h>
#include <vector>
#include "ICommandContext.h"

class ICommand {
public:
    virtual const char* getName() const = 0;
    virtual const char* getDescription() const = 0;
    virtual bool execute(const std::vector<String>& args, ICommandContext& context) = 0;
    virtual ~ICommand() = default;
};

#endif