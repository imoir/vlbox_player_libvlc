#ifndef VLBOX_PLAYER_HELPERS_H
#define VLBOX_PLAYER_HELPERS_H

#include <string>
#include <vector>

std::string get_working_path();
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
void trim(std::string& str);

enum class CommandType {
    Quit,                                               // all
    Video, Image, Black, Debug, None, Rainbow, Polygon, // video player
    Audio, Silence                                      // audio player
};

#endif //VLBOX_PLAYER3_HELPERS_H
