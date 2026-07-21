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

static bool _bmpPresent = false, _ahtPresent = false, _tmpPresent = false, _imuPresent = false;

void Sensors::begin() {
    _bmpPresent = bmp580.init();
    _ahtPresent = aht20.init();
    _tmpPresent = tmp102.init();
    _imuPresent = imu.init();
    bool gnssOk = gnss.init();
    Serial.println(_bmpPresent ? "Sensors: BMP580 OK"  : "Sensors: BMP580 not found");
    Serial.println(_ahtPresent ? "Sensors: AHT20 OK"   : "Sensors: AHT20 not found");
    Serial.println(_tmpPresent ? "Sensors: TMP102 OK"  : "Sensors: TMP102 not found");
    Serial.println(_imuPresent ? "Sensors: IMU OK"     : "Sensors: IMU not found");
    Serial.println(gnssOk       ? "Sensors: GNSS OK"    : "Sensors: GNSS not found (optional)");
}

bool Sensors::bmp580_present() { return _bmpPresent; }
bool Sensors::aht20_present()  { return _ahtPresent; }
bool Sensors::tmp102_present() { return _tmpPresent; }
bool Sensors::imu_present()    { return _imuPresent; }

float Sensors::temperature()           { bmp580.read(_temp, _press); return _temp; }
float Sensors::pressure()              { bmp580.read(_temp, _press); return _press; }
float Sensors::humidity()              { aht20.read(_ahtTemp, _hum); return _hum; }
float Sensors::temperature_secondary() { tmp102.read(_temp2); return _temp2; }

float Sensors::accel_x() { imu.read(_imu); return _imu.ax; }
float Sensors::accel_y() { imu.read(_imu); return _imu.ay; }
float Sensors::accel_z() { imu.read(_imu); return _imu.az; }

float Sensors::gyro_x()  { imu.read(_imu); return _imu.gx; }
float Sensors::gyro_y()  { imu.read(_imu); return _imu.gy; }
float Sensors::gyro_z()  { imu.read(_imu); return _imu.gz; }

bool Sensors::gnss_available()      { gnss.read(_gnss); return _gnss.present; }
const char* Sensors::gnss_sentence(){ gnss.read(_gnss); return _gnss.nmea; }

Sensors sensors;
