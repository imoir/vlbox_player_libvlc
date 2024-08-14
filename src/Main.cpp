#include <chrono>
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

int main() {
    cout << "[MAIN] Intenscity Player starting..." << endl;

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
    if (instance == nullptr) {
        cerr << "[MAIN] ERROR: Can't init libvlc." << endl;
        return -4;
    }
    cout << "[MAIN] a" << endl;
    play("black.jpg");

    cout << "[MAIN] init Commander" << endl;
    Commander commander(configuration);
    if(!commander.init()) {
        cerr << "[MAIN] ERROR: Can't init Commander." << endl;
        return -4;
    }

    cout << "[MAIN] Enter loop (q to quit)" << endl;

    bool quit = false;
    string nextCommand;

    while (!quit){

        // read the command
        if(commander.getNextCommand(nextCommand)) {
            cout << "[MAIN] Next Command: " << nextCommand << endl;
            execute(nextCommand, quit, configuration);
        }
    }

    cout << "[MAIN] Clean...";
    libvlc_release(instance);

    cout << "[MAIN] Done.";
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
        play("black.jpg");
    }
    else if(command == "quit") {
        stop();
        quit = true;
    }
    else {
        cerr << "[MAIN] unknown command: " << command << endl;
    }
}

void play(const std::string &file) {
    cout << "[MAIN] b" << endl;
    stop();
    cout << "[MAIN] c" << endl;

    std::string filePath = mediaDir + file;
    cout << "[MAIN] " << filePath << endl;

    libvlc_media_t *media = libvlc_media_new_path(instance, filePath.c_str());
    cout << "[MAIN] d" << endl;
    mediaPlayer = libvlc_media_player_new_from_media(media);
    cout << "[MAIN] e" << endl;
    libvlc_media_release(media);
    cout << "[MAIN] f" << endl;
    libvlc_set_fullscreen(mediaPlayer, true);
    cout << "[MAIN] g" << endl;
    libvlc_media_player_play(mediaPlayer);
    cout << "[MAIN] h" << endl;
}

void stop() {
    cout << "[MAIN] 1" << endl;
    if (mediaPlayer != nullptr) {
    cout << "[MAIN] 2" << endl;
        libvlc_media_player_stop(mediaPlayer);
    cout << "[MAIN] 3" << endl;
        libvlc_media_player_release(mediaPlayer);
    cout << "[MAIN] 4" << endl;
        mediaPlayer = nullptr;
    cout << "[MAIN] 5" << endl;
    }
}
