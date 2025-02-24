#include "managers/LogManager.h"

const char* LogManager::LOG_TOPIC_PREFIX = "devices";

const char* LogManager::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool LogManager::isLogLevelEnabled(LogLevel level) {
    return static_cast<int>(level) >= static_cast<int>(logLevel);
}

void LogManager::log(LogLevel level, const char* source, const char* message) {
    if (static_cast<int>(logLevel) < 0 || static_cast<int>(logLevel) > getLogLevelCount()-1) {
        logLevel = LogLevel::INFO;
        return;
    }

    if (!isLogLevelEnabled(level)) {
        return;
    }

    unsigned long ms = millis();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "[%lu] %s: %s: %s", ms, getLogLevelString(level), source, message);

    Serial.println(msgBuffer);
}

void LogManager::debug(const char* source, const char* message) {
    log(LogLevel::DEBUG, source, message);
}

void LogManager::info(const char* source, const char* message) {
    log(LogLevel::INFO, source, message);
}

void LogManager::warning(const char* source, const char* message) {
    log(LogLevel::WARNING, source, message);
}

void LogManager::error(const char* source, const char* message) {
    log(LogLevel::ERROR, source, message);
}