#include "bme280.hpp"
#include "config.hpp"
#include <Arduino.h>
#include <Wire.h>

// BME280 registers
#define REG_CHIPID    0xD0
#define REG_SOFTRESET 0xE0
#define REG_CTRL_HUM  0xF2
#define REG_STATUS    0xF3
#define REG_CTRL_MEAS 0xF4
#define REG_CONFIG    0xF5
#define REG_PRESS_MSB 0xF7
#define REG_DIG_T1    0x88
#define REG_DIG_H1    0xA1
#define REG_DIG_H2    0xE1

static uint8_t readReg(uint8_t reg) {
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)BME280_ADDR, (uint8_t)1);
    return Wire.read();
}

static uint16_t readReg16LE(uint8_t reg) {
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)BME280_ADDR, (uint8_t)2);
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    return (uint16_t)(msb << 8) | lsb;
}

static void writeReg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void BME280::_readCalibration() {
    _dig_T1 = readReg16LE(REG_DIG_T1);
    _dig_T2 = (int16_t)readReg16LE(REG_DIG_T1 + 2);
    _dig_T3 = (int16_t)readReg16LE(REG_DIG_T1 + 4);

    _dig_P1 = readReg16LE(REG_DIG_T1 + 6);
    _dig_P2 = (int16_t)readReg16LE(REG_DIG_T1 + 8);
    _dig_P3 = (int16_t)readReg16LE(REG_DIG_T1 + 10);
    _dig_P4 = (int16_t)readReg16LE(REG_DIG_T1 + 12);
    _dig_P5 = (int16_t)readReg16LE(REG_DIG_T1 + 14);
    _dig_P6 = (int16_t)readReg16LE(REG_DIG_T1 + 16);
    _dig_P7 = (int16_t)readReg16LE(REG_DIG_T1 + 18);
    _dig_P8 = (int16_t)readReg16LE(REG_DIG_T1 + 20);
    _dig_P9 = (int16_t)readReg16LE(REG_DIG_T1 + 22);

    _dig_H1 = readReg(REG_DIG_H1);
    _dig_H2 = (int16_t)readReg16LE(REG_DIG_H2);
    _dig_H3 = readReg(REG_DIG_H2 + 2);

    int8_t h4_msb = readReg(REG_DIG_H2 + 3);
    int8_t h4_lsb = readReg(REG_DIG_H2 + 4);
    int8_t h5_msb = readReg(REG_DIG_H2 + 5);
    _dig_H4 = (h4_msb << 4) | (h4_lsb & 0x0F);
    _dig_H5 = (h5_msb << 4) | (h4_lsb >> 4);
    _dig_H6 = (int8_t)readReg(REG_DIG_H2 + 6);
}

float BME280::_compensateTemp(int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1))) * ((int32_t)_dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)_dig_T1)) * ((adc_T >> 4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;
    _t_fine = var1 + var2;
    return ((float)((_t_fine * 5 + 128) >> 8)) / 100.0f;
}

float BME280::_compensatePressure(int32_t adc_P) {
    int64_t var1 = ((int64_t)_t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)_dig_P6;
    var2 = var2 + ((var1 * (int64_t)_dig_P5) << 17);
    var2 = var2 + (((int64_t)_dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_dig_P3) >> 8) + ((var1 * (int64_t)_dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_dig_P1) >> 33;
    if (var1 == 0) return 0.0f;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_dig_P8) * p) >> 19;
    return (float)((p + var1 + var2) >> 8) / 256.0f;
}

float BME280::_compensateHumidity(int32_t adc_H) {
    int32_t v = _t_fine - 76800;
    v = (((((adc_H << 14) - (((int32_t)_dig_H4) << 20) - (((int32_t)_dig_H5) * v)) +
          16384) >> 15) *
        (((((((v * (int32_t)_dig_H6) >> 10) *
             (((v * (int32_t)_dig_H3) >> 11) + 32768)) >> 10) + 2097152) *
           (int32_t)_dig_H2 + 8192) >> 14));
    v -= (((((v >> 15) * (v >> 15)) >> 7) * (int32_t)_dig_H1) >> 4);
    if (v < 0) v = 0;
    if (v > 419430400) v = 419430400;
    return (float)(v >> 12) / 1024.0f;
}

bool BME280::init() {
    if (readReg(REG_CHIPID) != 0x60) return false;
    _readCalibration();
    writeReg(REG_CTRL_HUM,  0x01);  // humidity oversampling x1
    writeReg(REG_CTRL_MEAS, 0x00);  // sleep mode — read() triggers forced measurements
    return true;
}

bool BME280::read(float& temp, float& press, float& hum) {
    writeReg(REG_CTRL_MEAS, 0x25);  // forced mode, temp+press oversampling x1
    uint8_t timeout = 100;
    while ((readReg(REG_STATUS) & 0x08) && --timeout) delay(1);
    if (!timeout) return false;

    Wire.beginTransmission(BME280_ADDR);
    Wire.write(REG_PRESS_MSB);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)BME280_ADDR, (uint8_t)8);

    if (Wire.available() != 8) return false;

    uint8_t d[8];
    for (int i = 0; i < 8; i++) d[i] = Wire.read();

    int32_t adc_P = (d[0] << 12) | (d[1] << 4) | (d[2] >> 4);
    int32_t adc_T = (d[3] << 12) | (d[4] << 4) | (d[5] >> 4);
    int32_t adc_H = (d[6] << 8)  |  d[7];

    temp  = _compensateTemp(adc_T);
    press = _compensatePressure(adc_P) / 100.0f;
    hum   = _compensateHumidity(adc_H);
    return true;
}

BME280 bme;
