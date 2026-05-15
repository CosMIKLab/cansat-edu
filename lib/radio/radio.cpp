#include "radio.hpp"
#include "config.hpp"
#include <Arduino.h>

const char* Radio::_settings[RADIO_SETTINGS_LEN] = {
    "mod lora",
    "freq 868100000",
    "sf sf7",
    "pa off",
    "pwr 12"
};

bool Radio::init() {
    Serial1.begin(BAUD_RADIO);
    for (uint8_t i = 0; i < RADIO_SETTINGS_LEN; i++) {
        Serial1.print("radio set ");
        Serial1.print(_settings[i]);
        Serial1.print("\r\n");
    }
    return true;
}

bool Radio::send(const uint8_t* payload, uint8_t len) {
    Serial1.print("radio tx ");
    for (uint8_t i = 0; i < len; i++) {
        if (payload[i] < 0x10) Serial1.print('0');
        Serial1.print(payload[i], HEX);
    }
    Serial1.print("\r\n");
    return true;
}

Radio radio;
