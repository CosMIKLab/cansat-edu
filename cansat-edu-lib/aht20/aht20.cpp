#include "aht20.hpp"
#include <config.hpp>
#include <Arduino.h>
#include <Wire.h>

static bool _present = false;

bool AHT20::init() {
    Wire.beginTransmission(AHT20_ADDR);
    Wire.write((uint8_t)0xBE);
    Wire.write((uint8_t)0x08);
    Wire.write((uint8_t)0x00);
    _present = (Wire.endTransmission() == 0);
    delay(10);
    return _present;
}

bool AHT20::read(float& temp, float& hum) {
    if (!_present) return false;

    Wire.beginTransmission(AHT20_ADDR);
    Wire.write((uint8_t)0xAC);
    Wire.write((uint8_t)0x33);
    Wire.write((uint8_t)0x00);
    if (Wire.endTransmission() != 0) return false;
    delay(90);

    Wire.requestFrom((uint8_t)AHT20_ADDR, (uint8_t)6);
    if (Wire.available() != 6) return false;

    uint8_t raw[6];
    for (uint8_t i = 0; i < 6; i++) raw[i] = Wire.read();
    if (raw[0] & 0x80) return false;  // still busy

    uint32_t humidityRaw    = ((uint32_t)raw[1] << 12) | ((uint32_t)raw[2] << 4) | (raw[3] >> 4);
    uint32_t temperatureRaw = (((uint32_t)raw[3] & 0x0F) << 16) | ((uint32_t)raw[4] << 8) | raw[5];

    hum  = (float)humidityRaw * 100.0f / 1048576.0f;
    temp = (float)temperatureRaw * 200.0f / 1048576.0f - 50.0f;
    return true;
}

AHT20 aht20;
