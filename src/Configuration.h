#ifndef VLBOX_PLAYER3_CONFIGURATION_H
#define VLBOX_PLAYER3_CONFIGURATION_H

#include <string>

typedef struct
{
    bool debug;
    std::string namedPipe;
    std::string mediaDir;
    std::string name;
    std::string id;
    std::string mode;
    bool audio;
    int height;
} PlayerConfiguration;

bool readConfiguration(PlayerConfiguration &configuration);
void displayConfiguration(PlayerConfiguration configuration);

#endif // VLBOX_PLAYER3_CONFIGURATION_H
