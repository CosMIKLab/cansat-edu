#include <Arduino.h>
#include "board.hpp"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "radio.hpp"

void setup() {
    board.init();

    if (!bme.init()) {
        Serial.println("BME280 init failed! Check wiring.");
        while (1) delay(10);
    } else {
        Serial.println("BME280 initialized.");
    }

    if (!imu.init()) {
        Serial.println("LSM6DSOX init failed! Check wiring.");
    } else {
        Serial.println("LSM6DSOX initialized.");
    }

    if (!radio.init()) {
        Serial.println("Radio init failed!");
    } else {
        Serial.println("Radio initialized.");
    }
}

static uint32_t lastRead = 0;
static constexpr uint32_t READ_INTERVAL_MS = 2000;

void loop() {

    
    /*if (millis() - lastRead < READ_INTERVAL_MS) return;
    lastRead = millis();

    float temp, press, hum;
    if (bme.read(temp, press, hum)) {
        Serial.printf("T: %.2f C  P: %.2f hPa  H: %.2f%%\n", temp, press, hum);
    } else {
        Serial.println("BME280 read error.");
    }

    ImuData imuData;
    if (imu.read(imuData)) {
        Serial.printf("Accel [g]   X: %.3f  Y: %.3f  Z: %.3f\n", imuData.ax, imuData.ay, imuData.az);
        Serial.printf("Gyro  [dps] X: %.2f  Y: %.2f  Z: %.2f\n", imuData.gx, imuData.gy, imuData.gz);
    } else {
        Serial.println("LSM6DSOX read error.");
    }*/
}
