#pragma once
#include <Arduino.h>

class SdEasy {
public:
    void begin();
    void log(float a, float b, float c);
    void note(const char* msg);
};

extern SdEasy sd;
