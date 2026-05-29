#include "bme280.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "BME280";

#define REG_CHIPID    0xD0
#define REG_CTRL_HUM  0xF2
#define REG_STATUS    0xF3
#define REG_CTRL_MEAS 0xF4
#define REG_PRESS_MSB 0xF7
#define REG_DIG_T1    0x88
#define REG_DIG_H1    0xA1
#define REG_DIG_H2    0xE1

esp_err_t BME280::_readReg(uint8_t reg, uint8_t* data, size_t len) {
    return i2c_master_transmit_receive(_dev, &reg, 1, data, len, 100);
}

esp_err_t BME280::_writeReg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return i2c_master_transmit(_dev, buf, 2, 100);
}

void BME280::_readCalibration() {
    uint8_t buf[24];
    _readReg(REG_DIG_T1, buf, 24);

    _dig_T1 = (uint16_t)((buf[1] << 8) | buf[0]);
    _dig_T2 = (int16_t)((buf[3] << 8) | buf[2]);
    _dig_T3 = (int16_t)((buf[5] << 8) | buf[4]);
    _dig_P1 = (uint16_t)((buf[7] << 8) | buf[6]);
    _dig_P2 = (int16_t)((buf[9] << 8)  | buf[8]);
    _dig_P3 = (int16_t)((buf[11] << 8) | buf[10]);
    _dig_P4 = (int16_t)((buf[13] << 8) | buf[12]);
    _dig_P5 = (int16_t)((buf[15] << 8) | buf[14]);
    _dig_P6 = (int16_t)((buf[17] << 8) | buf[16]);
    _dig_P7 = (int16_t)((buf[19] << 8) | buf[18]);
    _dig_P8 = (int16_t)((buf[21] << 8) | buf[20]);
    _dig_P9 = (int16_t)((buf[23] << 8) | buf[22]);

    _readReg(REG_DIG_H1, &_dig_H1, 1);

    uint8_t hbuf[7];
    _readReg(REG_DIG_H2, hbuf, 7);
    _dig_H2 = (int16_t)((hbuf[1] << 8) | hbuf[0]);
    _dig_H3 = hbuf[2];
    _dig_H4 = (int16_t)((hbuf[3] << 4) | (hbuf[4] & 0x0F));
    _dig_H5 = (int16_t)((hbuf[5] << 4) | (hbuf[4] >> 4));
    _dig_H6 = (int8_t)hbuf[6];
}

float BME280::_compensateTemp(int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1))) * ((int32_t)_dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - (int32_t)_dig_T1) * ((adc_T >> 4) - (int32_t)_dig_T1)) >> 12) * (int32_t)_dig_T3) >> 14;
    _t_fine = var1 + var2;
    return (float)((_t_fine * 5 + 128) >> 8) / 100.0f;
}

float BME280::_compensatePressure(int32_t adc_P) {
    int64_t var1 = (int64_t)_t_fine - 128000;
    int64_t var2 = var1 * var1 * (int64_t)_dig_P6;
    var2 = var2 + ((var1 * (int64_t)_dig_P5) << 17);
    var2 = var2 + ((int64_t)_dig_P4 << 35);
    var1 = ((var1 * var1 * (int64_t)_dig_P3) >> 8) + ((var1 * (int64_t)_dig_P2) << 12);
    var1 = ((((int64_t)1 << 47) + var1) * (int64_t)_dig_P1) >> 33;
    if (var1 == 0) return 0.0f;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)_dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)_dig_P8 * p) >> 19;
    return (float)((p + var1 + var2) >> 8) / 256.0f;
}

float BME280::_compensateHumidity(int32_t adc_H) {
    int32_t v = _t_fine - 76800;
    v = (((((adc_H << 14) - ((int32_t)_dig_H4 << 20) - ((int32_t)_dig_H5 * v)) + 16384) >> 15) *
         (((((((v * (int32_t)_dig_H6) >> 10) * (((v * (int32_t)_dig_H3) >> 11) + 32768)) >> 10) + 2097152) * (int32_t)_dig_H2 + 8192) >> 14));
    v -= (((((v >> 15) * (v >> 15)) >> 7) * (int32_t)_dig_H1) >> 4);
    if (v < 0) v = 0;
    if (v > 419430400) v = 419430400;
    return (float)(v >> 12) / 1024.0f;
}

bool BME280::init(i2c_master_bus_handle_t bus) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = BME280_ADDR,
        .scl_speed_hz    = 400000,
    };
    if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to I2C bus");
        return false;
    }

    uint8_t chip_id;
    if (_readReg(REG_CHIPID, &chip_id, 1) != ESP_OK || chip_id != 0x60) {
        ESP_LOGE(TAG, "Wrong chip ID: 0x%02X", chip_id);
        return false;
    }

    _readCalibration();
    _writeReg(REG_CTRL_HUM,  0x01);  // humidity oversampling x1
    _writeReg(REG_CTRL_MEAS, 0x00);  // sleep mode; read() uses forced mode
    ESP_LOGI(TAG, "Initialized OK");
    return true;
}

bool BME280::read(float& temp, float& press, float& hum) {
    _writeReg(REG_CTRL_MEAS, 0x25);  // forced mode, temp+press oversampling x1

    uint8_t status;
    int timeout = 100;
    do {
        vTaskDelay(pdMS_TO_TICKS(1));
        _readReg(REG_STATUS, &status, 1);
    } while ((status & 0x08) && --timeout);
    if (!timeout) return false;

    uint8_t d[8];
    if (_readReg(REG_PRESS_MSB, d, 8) != ESP_OK) return false;

    int32_t adc_P = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
    int32_t adc_T = ((int32_t)d[3] << 12) | ((int32_t)d[4] << 4) | (d[5] >> 4);
    int32_t adc_H = ((int32_t)d[6] << 8)  |  d[7];

    temp  = _compensateTemp(adc_T);
    press = _compensatePressure(adc_P) / 100.0f;
    hum   = _compensateHumidity(adc_H);
    return true;
}
