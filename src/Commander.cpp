#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <log4cpp/Category.hh>
#include <string.h>

#include "Commander.h"

using namespace std;

#define OUT_PREFIX_ERROR    "[Commander] ERROR:"

Commander::Commander(PlayerConfiguration& configuration) {
    pipeName = configuration.namedPipe;
}

bool Commander::init() {
    int ret = mkfifo(pipeName.c_str(), 0666);
    if(ret != 0) {
        if(errno != EEXIST) {
            log4cpp::Category::getRoot().error("%s unable to make fifo: %s", OUT_PREFIX_ERROR, strerror(errno));
            return false;
        }
    }

    return openFifo();
}

bool Commander::openFifo() {
    commandFd = open(pipeName.c_str(), O_RDONLY);
    if(commandFd < 0) {
        log4cpp::Category::getRoot().error("%s unable to open fifo: %s", OUT_PREFIX_ERROR, strerror(errno));
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
            log4cpp::Category::getRoot().error("%s unable to read fifo: %s", OUT_PREFIX_ERROR, strerror(errno));
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