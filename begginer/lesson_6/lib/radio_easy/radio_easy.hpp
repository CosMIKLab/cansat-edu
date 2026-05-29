#ifndef RADIO_EASY_HPP
#define RADIO_EASY_HPP

#include <Arduino.h>

class RadioEasy {
public:
    void begin();
    void send(float a, float b, float c);
    void send(String msg);
};

extern RadioEasy radio;

#endif /* RADIO_EASY_HPP */
