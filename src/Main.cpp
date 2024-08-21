#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vlc/vlc.h>

#include "Helpers.h"
#include "Commander.h"
#include "Configuration.h"

static libvlc_instance_t *instance = nullptr;
static libvlc_media_player_t *mediaPlayer = nullptr;
static std::string mediaDir;

using namespace std;

void execute(const string& message, bool& quit, PlayerConfiguration& configuration);
void play(const std::string &file);
void stop();
void DumpOsRelease();

int main() {
    cout << "[MAIN] Intenscity Player starting..." << endl;

    DumpOsRelease();

    cout << "[MAIN] vlc version : " << libvlc_get_version() << endl;
    cout << "[MAIN] vlc compiler : " << libvlc_get_version() << endl;
    cout << "[MAIN] vlc changeset : " << libvlc_get_changeset() << endl;

    const char *displayEnv = getenv("DISPLAY");
    if(displayEnv != nullptr)
        cout << "[MAIN] DISPLAY=" << displayEnv << endl;
    else
        cout << "[MAIN] DISPLAY NOT SET!!!" << endl;

    cout << "[MAIN] Read configuration" << endl;
    PlayerConfiguration configuration;
    if(!readConfiguration(configuration)) {
        return -1;
    }
    if(configuration.debug) {
        displayConfiguration(configuration);
    }
    mediaDir = configuration.mediaDir;

    cout << "[MAIN] Create VideoPlayer" << endl;
    instance = libvlc_new(0, nullptr);
    if(instance == nullptr) {
        cerr << "[MAIN] ERROR: Can't init libvlc." << endl;
        return -4;
    }
    stop();

    cout << "[MAIN] init Commander" << endl;
    Commander commander(configuration);
    if(!commander.init()) {
        cerr << "[MAIN] ERROR: Can't init Commander." << endl;
        return -4;
    }

    cout << "[MAIN] Enter loop" << endl;

    bool quit = false;
    string nextCommand;

    while (!quit){

        // read the command
        if(commander.getNextCommand(nextCommand)) {
            cout << "[MAIN] Next Command: " << nextCommand << endl;
            execute(nextCommand, quit, configuration);
        }
    }

    cout << "[MAIN] Clean..." << endl;
    if(mediaPlayer != nullptr) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }
    libvlc_release(instance);

    cout << "[MAIN] Done." << endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------

// commandes possibles:
//  - play <scenario_name>
//  - stop
//  - quit (pour quitter complètement le player)

void execute(const string& message, bool& quit, PlayerConfiguration& configuration) {
    vector<string> parts;
    split(message, parts, ':');

    string command = parts[0];
    trim(command);

    cout << "[MAIN] command: [" << command << "]" << endl;

    if(command == "play") {
        play(parts[1]);
    }
    else if(command == "stop") {
        stop();
    }
    else if(command == "quit") {
        quit = true;
    }
    else {
        cerr << "[MAIN] unknown command: " << command << endl;
    }
}

void play(const std::string &file) {
    std::string filePath = mediaDir + file;
    cout << "[MAIN] play file : " << filePath << endl;

    libvlc_media_t *media = libvlc_media_new_path(instance, filePath.c_str());

    if(mediaPlayer == nullptr) {
        mediaPlayer = libvlc_media_player_new_from_media(media);
        libvlc_set_fullscreen(mediaPlayer, true);
    }
    else {
        libvlc_media_player_set_media(mediaPlayer, media);
    }
    libvlc_media_release(media);
    libvlc_media_player_play(mediaPlayer);
}

void stop() {
    play("black.jpg");
}

void DumpOsRelease() {
    cout << "[MAIN] OS release : " << endl;

    ifstream file("/etc/os-release");
    string line;

    if (file.is_open())
    {
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
}
