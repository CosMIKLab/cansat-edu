#pragma once
#include <Arduino.h>

class RadioEasy {
public:
    void begin();
    void send(float a, float b, float c);
    void send(String msg);
};

extern RadioEasy radio;
