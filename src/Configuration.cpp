#include <iostream>
#include <fstream>
#include <string>

#include "Configuration.h"

using namespace std;

bool readConfiguration(PlayerConfiguration& configuration) {
    fstream file;

    const char *configurationFile = getenv("VLBOX_CONFIGURATION");
    if(configurationFile == nullptr) {
        cerr << "[CONFIGURATION] Unable to find the environment variable VLBOX_CONFIGURATION" << endl;
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
                cerr << "[MAIN] ERROR: Configuration error at line " << lineNumber << ": no =" << endl;
            }
            lineNumber++;
        }
        file.close();
    }

    return true;
}

void displayConfiguration(PlayerConfiguration configuration) {
    cout << "Player configuration: " << endl;
    cout << "\t- debug: " << (configuration.debug ? "true" : "false") << endl;
    cout << "\t- named pipe: " << configuration.namedPipe << endl;
    cout << "\t- media dir: " << configuration.mediaDir << endl;
    cout << "\t- vlbox name: " << configuration.name << endl;
    cout << "\t- vlbox id: " << configuration.id << endl;
    cout << "\t- vlbox mode: " << configuration.mode << endl;
    cout << "\t- audio: " << (configuration.audio ? "true" : "false") << endl;
}