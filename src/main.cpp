#include <Arduino.h>
#include "board.hpp"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "radio.hpp"
#include "storage.hpp"

void setup() {
    board.init();

    if (!bme.init()) {
        Serial.println("BME280 init failed! Check wiring.");
        while (1) delay(10);
    } else {
        Serial.println("BME280 initialized.");
    }

    bool imuOk = imu.init();
    if (!imuOk) Serial.println("LSM6DSOX init failed! Check wiring.");
    else        Serial.println("LSM6DSOX initialized.");

    bool radioOk = radio.init();
    if (!radioOk) Serial.println("Radio init failed!");
    else          Serial.println("Radio initialized.");

    if (!Storage::init(PIN_SD_CS)) {
        Serial.println("SD card init failed! Logging disabled.");
    } else {
        Serial.println("SD card ready.");
        Storage::event(imuOk   ? "LSM6DSOX OK"   : "LSM6DSOX FAIL");
        Storage::event(radioOk ? "RADIO OK"       : "RADIO FAIL");
    }
}

static uint32_t lastRead = 0;
static constexpr uint32_t READ_INTERVAL_MS = 2000;

void loop() {

    
    /*if (millis() - lastRead < READ_INTERVAL_MS) return;
    lastRead = millis();

    TelemetryRecord rec = {};
    rec.time_ms = millis();

    if (bme.read(rec.temp, rec.pressure, rec.humidity)) {
        Serial.printf("T: %.2f C  P: %.2f hPa  H: %.2f%%\n", rec.temp, rec.pressure, rec.humidity);
    } else {
        Serial.println("BME280 read error.");
    }

    ImuData imuData;
    if (imu.read(imuData)) {
        rec.ax = imuData.ax; rec.ay = imuData.ay; rec.az = imuData.az;
        rec.gx = imuData.gx; rec.gy = imuData.gy; rec.gz = imuData.gz;
        Serial.printf("Accel [g]   X: %.3f  Y: %.3f  Z: %.3f\n", rec.ax, rec.ay, rec.az);
        Serial.printf("Gyro  [dps] X: %.2f  Y: %.2f  Z: %.2f\n", rec.gx, rec.gy, rec.gz);
    } else {
        Serial.println("LSM6DSOX read error.");

    }

    Storage::log(rec);

    }*/

}
