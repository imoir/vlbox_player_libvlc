#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#include "Commander.h"

using namespace std;

#define OUT_PREFIX          "[Commander] "
#define OUT_PREFIX_ERROR    "[Commander] ERROR: "

Commander::Commander(PlayerConfiguration& configuration) {
    pipeName = configuration.namedPipe;
}

bool Commander::init() {
    int ret = mkfifo(pipeName.c_str(), 0666);
    if(ret != 0) {
        if(errno != EEXIST) {
            cerr << OUT_PREFIX_ERROR << "unable to make fifo: " << strerror(errno) << endl;
            return false;
        }
    }

    return openFifo();
}

bool Commander::openFifo() {
    commandFd = open(pipeName.c_str(), O_RDONLY);
    if(commandFd < 0) {
        cerr << OUT_PREFIX_ERROR << "unable to open fifo: " << strerror(errno) << endl;
        return false;
    }
    return true;
}

void Commander::clean() {
    if(commandFd >= 0) {
        close(commandFd);
        commandFd = -1;
    }
}

bool Commander::getNextCommand(string& nextCommand) {
    char buffer[1024];
    ssize_t size = read(commandFd, buffer, sizeof(buffer));
    // cout << OUT_PREFIX << "read: " << size << " (" << strerror(errno) << ")" << endl;
    if(size <= 0) {
        if(size != 0 && (size != -1 || errno != EAGAIN)) {
            cerr << OUT_PREFIX_ERROR << "unable to read fifo: " << strerror(errno) << endl;
        }
    }
    else {
        currentCommand += string(buffer, size);
        size_t idx = currentCommand.find('\n');
        if(idx != string::npos) {
            nextCommand = currentCommand.substr(0, idx);
            currentCommand = currentCommand.substr(idx + 1);
            return true;
        }
    }

    return false;
}