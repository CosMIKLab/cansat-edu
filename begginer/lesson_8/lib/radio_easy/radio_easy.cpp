#include "radio_easy.hpp"
#include <Arduino.h>

static const char* _settings[] = {
    "mod lora",
    "freq 868100000",
    "sf sf7",
    "pa off",
    "pwr 12"
};

void RadioEasy::begin() {
    for (uint8_t i = 0; i < 5; i++) {
        Serial.print("radio set ");
        Serial.print(_settings[i]);
        Serial.print("\r\n");
    }
    Serial.println("Radio: LoRa ready");
}

void RadioEasy::send(String msg) {
    Serial.print("radio tx ");
    for (uint8_t i = 0; i < msg.length(); i++) {
        uint8_t b = (uint8_t)msg[i];
        if (b < 0x10) Serial.print('0');
        Serial.print(b, HEX);
    }
    Serial.print("\r\n");
}

void RadioEasy::send(float a, float b, float c) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f,%.2f,%.2f", a, b, c);
    send(String(buf));
}

RadioEasy radio;
