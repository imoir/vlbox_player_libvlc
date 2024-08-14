#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Helpers.h"

using namespace std;

#define MAXPATHLEN 1024

std::string get_working_path()
{
    char temp[MAXPATHLEN];
    return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}

size_t split(const string &txt, vector<string> &strs, char ch) {
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != string::npos ) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, min( pos, txt.size() ) - initialPos + 1));

    return strs.size();
}

void trim(string& str) {
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}
