#include "lsm6ds3.hpp"
#include "config.h"
#include "esp_log.h"

static const char* TAG = "LSM6DS3";

#define REG_WHO_AM_I 0x0F
#define REG_CTRL1_XL 0x10
#define REG_CTRL2_G  0x11
#define REG_OUTX_L_G 0x22

#define WHOAMI_EXPECTED            0x69
#define WHOAMI_OBSERVED_COMPATIBLE 0x6A

esp_err_t LSM6DS3::_readReg(uint8_t reg, uint8_t* data, size_t len) {
    return i2c_master_transmit_receive(_dev, &reg, 1, data, len, 100);
}

esp_err_t LSM6DS3::_writeReg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return i2c_master_transmit(_dev, buf, 2, 100);
}

static bool supportedWhoAmI(uint8_t value) {
    return value == WHOAMI_EXPECTED || value == WHOAMI_OBSERVED_COMPATIBLE;
}

bool LSM6DS3::init(i2c_master_bus_handle_t bus) {
    _address = LSM6DS3_ADDR;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = _address,
        .scl_speed_hz    = 400000,
    };
    if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to I2C bus");
        return false;
    }

    uint8_t who = 0;
    if (_readReg(REG_WHO_AM_I, &who, 1) != ESP_OK || !supportedWhoAmI(who)) {
        // documented address didn't answer — retry at the fallback address
        i2c_master_bus_rm_device(_dev);
        _address = LSM6DS3_ADDR_ALT;
        dev_cfg.device_address = _address;
        if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to add device to I2C bus (fallback address)");
            return false;
        }
        if (_readReg(REG_WHO_AM_I, &who, 1) != ESP_OK || !supportedWhoAmI(who)) {
            ESP_LOGE(TAG, "Wrong WHO_AM_I: 0x%02X", who);
            return false;
        }
    }

    _writeReg(REG_CTRL1_XL, 0x60);  // accel: 416 Hz, ±2 g
    _writeReg(REG_CTRL2_G,  0x60);  // gyro:  416 Hz, 245 dps
    _ready = true;
    ESP_LOGI(TAG, "Initialized OK at 0x%02X", _address);
    return true;
}

bool LSM6DS3::read(ImuData& data) {
    if (!_ready) return false;

    uint8_t raw[12];
    if (_readReg(REG_OUTX_L_G, raw, 12) != ESP_OK) return false;

    int16_t raw_gx = (int16_t)((raw[1]  << 8) | raw[0]);
    int16_t raw_gy = (int16_t)((raw[3]  << 8) | raw[2]);
    int16_t raw_gz = (int16_t)((raw[5]  << 8) | raw[4]);
    int16_t raw_ax = (int16_t)((raw[7]  << 8) | raw[6]);
    int16_t raw_ay = (int16_t)((raw[9]  << 8) | raw[8]);
    int16_t raw_az = (int16_t)((raw[11] << 8) | raw[10]);

    data.ax = raw_ax * ACCEL_SENS;
    data.ay = raw_ay * ACCEL_SENS;
    data.az = raw_az * ACCEL_SENS;
    data.gx = raw_gx * GYRO_SENS;
    data.gy = raw_gy * GYRO_SENS;
    data.gz = raw_gz * GYRO_SENS;
    return true;
}
