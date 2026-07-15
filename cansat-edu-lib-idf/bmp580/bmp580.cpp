#include "bmp580.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "BMP580";

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

esp_err_t BMP580::_readReg(uint8_t reg, uint8_t* data, size_t len) {
    return i2c_master_transmit_receive(_dev, &reg, 1, data, len, 100);
}

esp_err_t BMP580::_writeReg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return i2c_master_transmit(_dev, buf, 2, 100);
}

void BMP580::_setPowerMode(uint8_t mode) {
    _writeReg(REG_ODR_CONFIG, DEEP_STANDBY_DISABLED | ODR_10_HZ | mode);
}

bool BMP580::init(i2c_master_bus_handle_t bus) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = BMP580_ADDR,
        .scl_speed_hz    = 400000,
    };
    if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to I2C bus");
        return false;
    }

    uint8_t chip_id;
    if (_readReg(REG_CHIP_ID, &chip_id, 1) != ESP_OK ||
        (chip_id != CHIP_ID_PRIMARY && chip_id != CHIP_ID_SECONDARY)) {
        ESP_LOGE(TAG, "Wrong chip ID: 0x%02X", chip_id);
        return false;
    }

    _setPowerMode(POWERMODE_STANDBY);
    _writeReg(REG_OSR_CONFIG, PRESSURE_ENABLE | PRESS_OSR_4X | TEMP_OSR_2X);
    _setPowerMode(POWERMODE_STANDBY);
    _present = true;
    ESP_LOGI(TAG, "Initialized OK");
    return true;
}

bool BMP580::read(float& temp, float& press) {
    if (!_present) return false;

    _setPowerMode(POWERMODE_FORCED);
    vTaskDelay(pdMS_TO_TICKS(12));

    uint8_t d[6];
    if (_readReg(REG_TEMP_XLSB, d, sizeof(d)) != ESP_OK) return false;

    uint32_t rawTempU = ((uint32_t)d[2] << 16) | ((uint32_t)d[1] << 8) | d[0];
    int32_t  rawTemp  = (rawTempU & 0x800000UL) ? (int32_t)(rawTempU | 0xFF000000UL) : (int32_t)rawTempU;
    uint32_t rawPress = ((uint32_t)d[5] << 16) | ((uint32_t)d[4] << 8) | d[3];

    temp  = (float)rawTemp / 65536.0f;
    press = (float)rawPress / 64.0f / 100.0f;  // Pa -> hPa
    return true;
}
