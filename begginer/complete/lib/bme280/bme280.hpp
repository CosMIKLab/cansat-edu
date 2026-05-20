#pragma once
#include <stdint.h>

class BME280 {
public:
    bool init();
    bool read(float& temp, float& press, float& hum);

private:
    int32_t  _t_fine = 0;

    uint16_t _dig_T1;
    int16_t  _dig_T2, _dig_T3;

    uint16_t _dig_P1;
    int16_t  _dig_P2, _dig_P3, _dig_P4, _dig_P5, _dig_P6, _dig_P7, _dig_P8, _dig_P9;

    uint8_t  _dig_H1, _dig_H3;
    int16_t  _dig_H2, _dig_H4, _dig_H5;
    int8_t   _dig_H6;

    void  _readCalibration();
    float _compensateTemp(int32_t adc_T);
    float _compensatePressure(int32_t adc_P);
    float _compensateHumidity(int32_t adc_H);
};

extern BME280 bme;
