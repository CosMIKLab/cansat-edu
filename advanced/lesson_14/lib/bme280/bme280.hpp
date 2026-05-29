#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

class BME280 {
public:
    bool init(i2c_master_bus_handle_t bus);
    bool read(float& temp, float& press, float& hum);

private:
    i2c_master_dev_handle_t _dev = nullptr;
    int32_t  _t_fine = 0;

    uint16_t _dig_T1;
    int16_t  _dig_T2, _dig_T3;
    uint16_t _dig_P1;
    int16_t  _dig_P2, _dig_P3, _dig_P4, _dig_P5, _dig_P6, _dig_P7, _dig_P8, _dig_P9;
    uint8_t  _dig_H1, _dig_H3;
    int16_t  _dig_H2, _dig_H4, _dig_H5;
    int8_t   _dig_H6;

    esp_err_t _readReg(uint8_t reg, uint8_t* data, size_t len);
    esp_err_t _writeReg(uint8_t reg, uint8_t value);
    void      _readCalibration();
    float     _compensateTemp(int32_t adc_T);
    float     _compensatePressure(int32_t adc_P);
    float     _compensateHumidity(int32_t adc_H);
};
