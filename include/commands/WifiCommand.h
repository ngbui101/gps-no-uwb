#ifndef WIFI_COMMAND_H
#define WIFI_COMMAND_H

#include "interfaces/IExtendedCommand.h"
#include "managers/ConfigManager.h"
#include "managers/WifiManager.h"

class WifiCommand : public IExtendedCommand {
private:
    WifiManager& wifiManager;
    ConfigManager& configManager;
    LogManager& log;

    bool startAccessPointCmd(const std::vector<String>& args, ICommandContext& context);

public:
    WifiCommand() 
        : wifiManager(WifiManager::getInstance())
        , log(LogManager::getInstance())
        , configManager(ConfigManager::getInstance()) {
            subCommands["start"] = [this](const std::vector<String>& args, ICommandContext& context) {
                return startAccessPointCmd(args, context);
            };

            subCommandDescriptions["start"] = "Starts the WiFi access point";

            subCommandParameters["start"] = {
                {"--ssid", "SSID of the target AP", true, ""},
                {"--password", "Password of the target AP - leave it empty if it is a public network", false, ""}
            };
        }

        const char* getName() const override {
            return "wifi";
        }
    
        const char* getDescription() const override {
            return "Configure and manage WiFi";
        }
};

#endif