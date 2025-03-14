#include "managers/SerialManager.h"

bool SerialManager::begin() {
    log.debug("SerialManager", "Initializing SerialManager...");
    showPrompt();

    return true;
}

void SerialManager::update() {
    if(Serial.available()) {
        char c = (char)Serial.read();
        processInput(c);
    }
}

void SerialManager::showPrompt() {
    Serial.print("\n\r#> ");
    isPromptDisplayed = true;
}

void SerialManager::processInput(char c) {
    if(ignoreNextInputCycle) {
        ignoreNextInputCycle = false;
        return;
    }

    if(c == '\n' || c == '\r') {   
        executeCommand();
    } else if (c == 127 || c == 8) {
        handleBackspace();
    } else {
        inputBuffer += c;
        Serial.print(c);
    }

}

void SerialManager::executeCommand() {
    ignoreNextInputCycle = true; 
    
    if(inputBuffer.length() > 0) {
        Serial.println();
        commandManager.executeCommand(inputBuffer, context);
        inputBuffer = "";
    } 

    showPrompt();
}

void SerialManager::handleBackspace() {
    if(inputBuffer.length() > 0) {
        inputBuffer.remove(inputBuffer.length() - 1);
        Serial.print("\b \b");
    }
}