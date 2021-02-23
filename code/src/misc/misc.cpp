#include "../../headers/misc/misc.h"
#ifdef __linux__
#include <sys/time.h>
#endif
#include <ctime>
#include <cstdlib>

#define RANDOM_VARIANTS 70

static char rstring_palette[RANDOM_VARIANTS + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&*(";

void genRandomString(std::string* result, unsigned int len){
    result->clear(); //make string empty, first
    srand (static_cast<unsigned int>(SDL_GetPerformanceCounter()));
    unsigned int _len = len + rand() % 3;
    for(unsigned int digit_i = 0; digit_i < _len; digit_i ++){//Iterate over all chars in string

        int v = rand() % RANDOM_VARIANTS; //Get random value

        result->push_back(rstring_palette[v]); //Push symbol at random value
     }
}

void readString(std::string& str, const char* data, unsigned int& offset) {
    str.clear();
    while (data[offset] != '\0') {
        str += data[offset];
        offset++;
    }
    offset++;
}

void readLine(std::string& str, const char* data, unsigned int& offset) {
    str.clear();
    while (data[offset] != '\n') {
        str += data[offset];
        offset++;
    }
    offset++;
}

void RemoveExtension(std::string& str) {
    while (str[str.size() - 1] != '.') {
        str.pop_back();
    }
    str.pop_back();
}

void GetFileName(std::string& str) {
    std::string old = str;
    str.clear();
    int last_slash = static_cast<int>(old.size()) - 1;
    while (old[last_slash] != '/')
        last_slash--;
    for (int i = last_slash + 1; i < old.size(); i++) {
        str.push_back(old[i]);
    }
}

void ProcessFileName(std::string& str) {
    for (int i = 0; i < str.size(); i++) {
        char c = str[i];
        if (c == '|' || c == ':' || c == '/' || c == '\\' || c == '*')
            str[i] = '_';
    }
}

void skipSpaces(const char* data, unsigned int& offset) {
    while (data[offset] == ' ' || data[offset] == '\n') {
        offset++;
    }
}

bool startsWith(std::string& s, std::string m) {
    if (s.size() < m.size())
        return false;
    bool result = true;
    for (unsigned i = 0; i < m.size(); i++) {
        if (s[i] != m[i])
            return false;
    }
    return true;
}

void ZsStream::writeString(std::string str) {
    char term = '\0';
    unsigned int size = static_cast<unsigned int>(str.size());
    write(str.c_str(), size);
    write(&term, 1);
}