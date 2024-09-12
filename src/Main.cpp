#include <chrono>
#include <fstream>
#include <log4cpp/Category.hh>
#include <log4cpp/SyslogAppender.hh>
#include <cstdio>
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

static void libvlc_log_callback(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args)
{
    log4cpp::Category &logger = log4cpp::Category::getRoot();
    int logLevel = logger.getPriority();
    switch (level)
    {
    case LIBVLC_DEBUG:
        if (logLevel < log4cpp::Priority::DEBUG)
            return;
        break;
    case LIBVLC_NOTICE:
        if (logLevel < log4cpp::Priority::INFO)
            return;
        break;
    case LIBVLC_WARNING:
        if (logLevel < log4cpp::Priority::WARN)
            return;
        break;
    case LIBVLC_ERROR:
        if (logLevel < log4cpp::Priority::ERROR)
            return;
        break;
    }

    char *bufferToUse = nullptr;
    char *allocatedBuffer = nullptr;
    constexpr const int BufferLength = 200;
    char buffer[BufferLength];
    int len = vsnprintf(buffer, BufferLength, fmt, args);
    if (len >= BufferLength)
    {
        allocatedBuffer = new char[len + 1];
        vsnprintf(allocatedBuffer, len + 1, fmt, args);
        bufferToUse = allocatedBuffer;
    }
    else
    {
        bufferToUse = buffer;
    }

    switch (level)
    {
    case LIBVLC_DEBUG:
        logger.debug("[VLCLIB] %s", bufferToUse);
        break;
    case LIBVLC_NOTICE:
        logger.info("[VLCLIB] %s", bufferToUse);
        break;
    case LIBVLC_WARNING:
        logger.warn("[VLCLIB] %s", bufferToUse);
        break;
    case LIBVLC_ERROR:
        logger.error("[VLCLIB] %s", bufferToUse);
        break;
    }

    if (allocatedBuffer != nullptr)
        delete allocatedBuffer;
}

int main() {
    log4cpp::Appender *logAppender = new log4cpp::SyslogAppender("player", "player");
    logAppender->setLayout(new log4cpp::BasicLayout());
    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.setPriority(log4cpp::Priority::INFO);
    logger.addAppender(logAppender);

    logger.info("[MAIN] Intenscity Player starting...");

    DumpOsRelease();

    logger.info("[MAIN] vlc version : %s", libvlc_get_version());
    logger.info("[MAIN] vlc compiler : %s", libvlc_get_compiler());
    logger.info("[MAIN] vlc changeset : %s", libvlc_get_changeset());

    const char *displayEnv = getenv("DISPLAY");
    if(displayEnv != nullptr)
        logger.info("[MAIN] DISPLAY=%s", displayEnv);
    else
        logger.info("[MAIN] DISPLAY NOT SET!!!");

    logger.info("[MAIN] Read configuration");
    PlayerConfiguration configuration;
    if(!readConfiguration(configuration)) {
        return -1;
    }
    if(configuration.debug) {
        displayConfiguration(configuration);
    }
    mediaDir = configuration.mediaDir;

    logger.info("[MAIN] Create VideoPlayer");
    instance = libvlc_new(0, nullptr);
    if(instance == nullptr) {
        logger.error("[MAIN] ERROR: Can't init libvlc.");
        return -4;
    }
    libvlc_log_set(instance, libvlc_log_callback, nullptr);
    stop();

    logger.info("[MAIN] init Commander");
    Commander commander(configuration);
    if(!commander.init()) {
        logger.error("[MAIN] ERROR: Can't init Commander.");
        return -4;
    }

    logger.info("[MAIN] Enter loop");

    bool quit = false;
    string nextCommand;

    while (!quit){

        // read the command
        if(commander.getNextCommand(nextCommand)) {
            logger.info("[MAIN] Next Command: %s", nextCommand.c_str());
            execute(nextCommand, quit, configuration);
        }
    }

    logger.info("[MAIN] Clean...");
    if(mediaPlayer != nullptr) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }
    libvlc_release(instance);

    logger.info("[MAIN] Done.");
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

    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.info("[MAIN] command: [%s]", command.c_str());

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
        logger.error("[MAIN] unknown command: %s", command.c_str());
    }
}

void play(const std::string &file) {
    std::string filePath = mediaDir + file;
    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.info("[MAIN] play file : %s", filePath.c_str());

    libvlc_media_t *media = libvlc_media_new_path(instance, filePath.c_str());

    if(mediaPlayer == nullptr) {
        mediaPlayer = libvlc_media_player_new_from_media(media);
        libvlc_set_fullscreen(mediaPlayer, true);
    }
    else {
        libvlc_set_fullscreen(mediaPlayer, true);
        libvlc_media_player_set_media(mediaPlayer, media);
    }
    libvlc_media_release(media);
    libvlc_media_player_play(mediaPlayer);
}

void stop() {
    play("../player/misc/media/black.jpg");
}

void DumpOsRelease() {
    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.info("[MAIN] OS release :");

    ifstream file("/etc/os-release");
    string line;

    if (file.is_open())
    {
        while (getline(file, line)) {
            logger.info(" - %s", line.c_str());
        }
        file.close();
    }
}
