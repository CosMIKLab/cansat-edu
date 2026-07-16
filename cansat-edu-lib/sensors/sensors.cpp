#include "sensors.hpp"
#include <bmp580.hpp>
#include <aht20.hpp>
#include <tmp102.hpp>
#include <lsm6ds3.hpp>
#include <gnss.hpp>
#include <Arduino.h>

static float    _temp = 0, _press = 0, _hum = 0, _temp2 = 0;
static float    _ahtTemp = 0;
static ImuData  _imu  = {};
static GnssData _gnss = {};

static void _refresh() {
    bmp580.read(_temp, _press);
    aht20.read(_ahtTemp, _hum);
    tmp102.read(_temp2);
    imu.read(_imu);
    gnss.read(_gnss);
}

void Sensors::begin() {
    bool bmpOk  = bmp580.init();
    bool ahtOk  = aht20.init();
    bool tmpOk  = tmp102.init();
    bool imuOk  = imu.init();
    bool gnssOk = gnss.init();
    Serial.println(bmpOk  ? "Sensors: BMP580 OK"  : "Sensors: BMP580 not found");
    Serial.println(ahtOk  ? "Sensors: AHT20 OK"   : "Sensors: AHT20 not found");
    Serial.println(tmpOk  ? "Sensors: TMP102 OK"  : "Sensors: TMP102 not found");
    Serial.println(imuOk  ? "Sensors: IMU OK"     : "Sensors: IMU not found");
    Serial.println(gnssOk ? "Sensors: GNSS OK"    : "Sensors: GNSS not found (optional)");
}

float Sensors::temperature()           { _refresh(); return _temp; }
float Sensors::pressure()              { return _press; }
float Sensors::humidity()              { return _hum; }
float Sensors::temperature_secondary() { return _temp2; }

float Sensors::accel_x() { return _imu.ax; }
float Sensors::accel_y() { return _imu.ay; }
float Sensors::accel_z() { return _imu.az; }

float Sensors::gyro_x()  { return _imu.gx; }
float Sensors::gyro_y()  { return _imu.gy; }
float Sensors::gyro_z()  { return _imu.gz; }

bool Sensors::gnss_available()      { return _gnss.present; }
const char* Sensors::gnss_sentence(){ return _gnss.nmea; }

Sensors sensors;
