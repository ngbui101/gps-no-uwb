#ifndef SERIAL_CLI_H
#define SERIAL_CLI_H

#include <Arduino.h>
#include "managers/CommandManager.h"
#include "managers/LogManager.h"
#include "contexts/SerialCommandContext.h"

class SerialManager {
private:
    SerialManager()
        : commandManager(CommandManager::getInstance())
        , log(LogManager::getInstance())
        , inputBuffer("")
        , ignoreNextInputCycle(false)
        , isPromptDisplayed(false) {}


    CommandManager& commandManager;
    LogManager& log;

    SerialCommandContext context;
    String inputBuffer;
    bool isPromptDisplayed;
    const char* prompt;
    bool ignoreNextInputCycle;
    
    void showPrompt();
    void processInput(char c);
    void executeCommand();
    void handleBackspace();

public:
    SerialManager(const SerialManager&) = delete;
    SerialManager& operator=(const SerialManager&) = delete;

    static SerialManager& getInstance() {
        static SerialManager instance;
        return instance;
    }

    bool begin();
    void update();

};

#endif
