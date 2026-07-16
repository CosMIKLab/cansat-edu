#include "bmp580.hpp"
#include <config.hpp>
#include <Arduino.h>
#include <Wire.h>

#define REG_CHIP_ID    0x01
#define REG_TEMP_XLSB  0x1D
#define REG_OSR_CONFIG 0x36
#define REG_ODR_CONFIG 0x37

#define CHIP_ID_PRIMARY        0x50
#define CHIP_ID_SECONDARY      0x51
#define POWERMODE_STANDBY      0x00
#define POWERMODE_FORCED       0x02
#define DEEP_STANDBY_DISABLED  0x80
#define PRESSURE_ENABLE        0x40
#define TEMP_OSR_2X            0x01
#define PRESS_OSR_4X           (0x02 << 3)
#define ODR_10_HZ              (0x17 << 2)

static bool _present = false;

static uint8_t readReg(uint8_t reg) {
    Wire.beginTransmission(BMP580_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)BMP580_ADDR, (uint8_t)1);
    return Wire.read();
}

static void readRegs(uint8_t reg, uint8_t* data, uint8_t len) {
    Wire.beginTransmission(BMP580_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)BMP580_ADDR, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) data[i] = Wire.read();
}

static void writeReg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(BMP580_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

static void setPowerMode(uint8_t mode) {
    writeReg(REG_ODR_CONFIG, DEEP_STANDBY_DISABLED | ODR_10_HZ | mode);
}

bool BMP580::init() {
    uint8_t chipId = readReg(REG_CHIP_ID);
    if (chipId != CHIP_ID_PRIMARY && chipId != CHIP_ID_SECONDARY) {
        _present = false;
        return false;
    }

    setPowerMode(POWERMODE_STANDBY);
    writeReg(REG_OSR_CONFIG, PRESSURE_ENABLE | PRESS_OSR_4X | TEMP_OSR_2X);
    setPowerMode(POWERMODE_STANDBY);
    _present = true;
    return true;
}

bool BMP580::read(float& temp, float& press) {
    if (!_present) return false;

    setPowerMode(POWERMODE_FORCED);
    delay(12);

    uint8_t d[6] = {};
    readRegs(REG_TEMP_XLSB, d, sizeof(d));

    uint32_t rawTempU = ((uint32_t)d[2] << 16) | ((uint32_t)d[1] << 8) | d[0];
    int32_t  rawTemp  = (rawTempU & 0x800000UL) ? (int32_t)(rawTempU | 0xFF000000UL) : (int32_t)rawTempU;
    uint32_t rawPress = ((uint32_t)d[5] << 16) | ((uint32_t)d[4] << 8) | d[3];

    temp  = (float)rawTemp / 65536.0f;
    press = (float)rawPress / 64.0f / 100.0f;  // Pa -> hPa
    return true;
}

BMP580 bmp580;
