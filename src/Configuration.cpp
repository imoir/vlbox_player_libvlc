#include "Configuration.h"

#include <fstream>
#include <log4cpp/Category.hh>
#include <nlohmann/json.hpp>

static const char *ConfigFilename = "/etc/vlbox/vlbox.conf";

bool readConfiguration(PlayerConfiguration &configuration)
{
    std::ifstream configurationFile(ConfigFilename);
    if (configurationFile.is_open() != true)
    {
        log4cpp::Category::getRoot().error("[CONFIGURATION] Unable to open config file - %s", ConfigFilename);
        return false;
    }

    nlohmann::json config = nlohmann::json::parse(configurationFile);
    nlohmann::json player = config["player"];

    configuration.debug = player["debug"];
    configuration.namedPipe = player["pipe"];
    configuration.mediaDir = player["media"];
    configuration.name = player["name"];
    configuration.id = player["id"];
    configuration.audio = player["audio"];
    configuration.mode = player["mode"];
    configuration.height = player["height"];

    return true;
}

void displayConfiguration(PlayerConfiguration configuration)
{
    log4cpp::Category &logger = log4cpp::Category::getRoot();

    logger.info("Player configuration:");
    logger.info("- debug: %s", (configuration.debug ? "true" : "false"));
    logger.info("- named pipe: %s", configuration.namedPipe.c_str());
    logger.info("- media dir: %s", configuration.mediaDir.c_str());
    logger.info("- vlbox name: %s", configuration.name.c_str());
    logger.info("- vlbox id: %s", configuration.id.c_str());
    logger.info("- vlbox mode: %s", configuration.mode.c_str());
    logger.info("- audio: %s", (configuration.audio ? "true" : "false"));
}
