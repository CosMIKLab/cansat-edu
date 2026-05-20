#pragma once
#include <bme280.hpp>
#include <lsm6dsox.hpp>

class Sensors {
public:
    void begin();

    float temperature();
    float pressure();
    float humidity();

    float accel_x();
    float accel_y();
    float accel_z();

    float gyro_x();
    float gyro_y();
    float gyro_z();

private:
    float _temp = 0, _press = 0, _hum = 0;
    ImuData _imu = {};
    void _refresh();
};

extern Sensors sensors;
