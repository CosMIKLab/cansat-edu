#include "sensors.hpp"
#include <bme280.hpp>
#include <lsm6dsox.hpp>
#include <Arduino.h>

static float   _temp = 0, _press = 0, _hum = 0;
static ImuData _imu  = {};

static void _refresh() {
    bme.read(_temp, _press, _hum);
    imu.read(_imu);
}

void Sensors::begin() {
    bool bme_ok = bme.init();
    bool imu_ok = imu.init();
    Serial.println(bme_ok ? "Sensors: BME280 OK" : "Sensors: BME280 not found");
    Serial.println(imu_ok ? "Sensors: IMU OK"    : "Sensors: IMU not found");
}

float Sensors::temperature() { _refresh(); return _temp; }
float Sensors::pressure()    { return _press; }
float Sensors::humidity()    { return _hum; }

float Sensors::accel_x() { return _imu.ax; }
float Sensors::accel_y() { return _imu.ay; }
float Sensors::accel_z() { return _imu.az; }

float Sensors::gyro_x()  { return _imu.gx; }
float Sensors::gyro_y()  { return _imu.gy; }
float Sensors::gyro_z()  { return _imu.gz; }

Sensors sensors;
