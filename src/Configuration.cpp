#include <log4cpp/Category.hh>
#include <fstream>
#include <string>

#include "Configuration.h"

using namespace std;

bool readConfiguration(PlayerConfiguration& configuration) {
    fstream file;

    const char *configurationFile = getenv("VLBOX_CONFIGURATION");
    if(configurationFile == nullptr) {
        log4cpp::Category::getRoot().error("[CONFIGURATION] Unable to find the environment variable VLBOX_CONFIGURATION");
        return false;
    }

    file.open(configurationFile, ios::in);
    if(file.is_open()) {
        string line;
        int lineNumber = 1;
        while(getline(file, line)) {
            string::size_type idx = line.find( '=', 0 );
            if(idx != string::npos) {
                const string key = line.substr(0, idx);
                const string value = line.substr(idx + 1);

                if(key == "debug") {
                    configuration.debug = value == "true";
                }
                else if(key == "pipe") {
                    configuration.namedPipe = value;
                }
                else if(key == "media") {
                    configuration.mediaDir = value;
                }
                else if(key == "name") {
                    configuration.name = value;
                }
                else if(key == "id") {
                    configuration.id = value;
                }
                else if(key == "audio") {
                    configuration.audio = value == "true";
                }
                else if(key == "mode") {
                    configuration.mode = value;
                }
            }
            else {
                log4cpp::Category::getRoot().error("[MAIN] ERROR: Configuration error at line %d: no=", lineNumber);
            }
            lineNumber++;
        }
        file.close();
    }

    return true;
}

void displayConfiguration(PlayerConfiguration configuration) {
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
