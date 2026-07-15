#include "tmp102.hpp"
#include <config.hpp>
#include <Arduino.h>
#include <Wire.h>

static bool _present = false;

bool TMP102::init() {
    Wire.beginTransmission(TMP102_ADDR);
    _present = (Wire.endTransmission() == 0);
    return _present;
}

bool TMP102::read(float& temp) {
    if (!_present) return false;

    Wire.beginTransmission(TMP102_ADDR);
    Wire.write((uint8_t)0x00);
    if (Wire.endTransmission(false) != 0) return false;
    Wire.requestFrom((uint8_t)TMP102_ADDR, (uint8_t)2);
    if (Wire.available() != 2) return false;

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t value = (int16_t)((msb << 4) | (lsb >> 4));
    if (value & 0x0800) value |= 0xF000;  // sign-extend 12-bit value

    temp = (float)value * 0.0625f;
    return true;
}

TMP102 tmp102;
