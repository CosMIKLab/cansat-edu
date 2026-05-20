#include "lsm6dsox.hpp"
#include "config.hpp"
#include <Arduino.h>
#include <Wire.h>

#define REG_WHO_AM_I  0x0F
#define REG_CTRL1_XL  0x10
#define REG_CTRL2_G   0x11
#define REG_OUTX_L_G  0x22

static uint8_t readReg(uint8_t reg) {
    Wire.beginTransmission(LSM_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)LSM_ADDR, (uint8_t)1);
    return Wire.read();
}

static void writeReg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(LSM_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

bool LSM6DSOX::init() {
    if (readReg(REG_WHO_AM_I) != 0x6C) return false;
    writeReg(REG_CTRL1_XL, 0x40);  // accel: 104 Hz, ±2 g
    writeReg(REG_CTRL2_G,  0x40);  // gyro:  104 Hz, ±250 dps
    _ready = true;
    return true;
}

bool LSM6DSOX::read(ImuData& data) {
    if (!_ready) return false;
    Wire.beginTransmission(LSM_ADDR);
    Wire.write(REG_OUTX_L_G);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)LSM_ADDR, (uint8_t)12);

    if (Wire.available() != 12) return false;

    uint8_t raw[12];
    for (int i = 0; i < 12; i++) raw[i] = Wire.read();

    int16_t raw_gx = (raw[1] << 8) | raw[0];
    int16_t raw_gy = (raw[3] << 8) | raw[2];
    int16_t raw_gz = (raw[5] << 8) | raw[4];
    int16_t raw_ax = (raw[7] << 8) | raw[6];
    int16_t raw_ay = (raw[9] << 8) | raw[8];
    int16_t raw_az = (raw[11] << 8) | raw[10];

    data.ax = raw_ax * ACCEL_SENS;
    data.ay = raw_ay * ACCEL_SENS;
    data.az = raw_az * ACCEL_SENS;
    data.gx = raw_gx * GYRO_SENS;
    data.gy = raw_gy * GYRO_SENS;
    data.gz = raw_gz * GYRO_SENS;
    return true;
}

LSM6DSOX imu;
