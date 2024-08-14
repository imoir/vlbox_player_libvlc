//
// Created by Sébastien Doncker on 30/04/2024.
//

#ifndef VLBOX_PLAYER3_COMMANDER_H
#define VLBOX_PLAYER3_COMMANDER_H

#include <string>

#include "Configuration.h"

class Commander {
private:
    std::string pipeName;
    int commandFd = -1;
    std::string currentCommand;

public:
    explicit Commander(PlayerConfiguration& configuration);

    bool init();
    void clean();
    bool getNextCommand(std::string& nextCommand);
private:
    bool openFifo();
};

#endif //VLBOX_PLAYER3_COMMANDER_H
