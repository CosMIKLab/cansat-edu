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
    for (uint8_t i = 0; i < RADIO_SETTINGS_LEN; i++) {
        Serial.print("radio set ");
        Serial.print(_settings[i]);
        Serial.print("\r\n");
    }
    return true;
}

bool Radio::send(const uint8_t* payload, uint8_t len) {
    Serial.print("radio tx ");
    for (uint8_t i = 0; i < len; i++) {
        if (payload[i] < 0x10) Serial.print('0');
        Serial.print(payload[i], HEX);
    }
    Serial.print("\r\n");
    return true;
}

Radio radio;
