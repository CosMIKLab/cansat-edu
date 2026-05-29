#ifndef SD_EASY_HPP
#define SD_EASY_HPP

#include <Arduino.h>

class SdEasy {
public:
    void begin();
    void log(float a, float b, float c);
    void note(const char* msg);
};

extern SdEasy sd;

#endif /* SD_EASY_HPP */
