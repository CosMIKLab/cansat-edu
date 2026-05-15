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
    }
    Serial.println("BME280 initialized.");

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

float temp, press, hum;
ImuData imuData;

void loop() {
    if (bme.read(temp, press, hum)) {
        Serial.printf("T: %.2f C  P: %.2f hPa  H: %.2f%%\n", temp, press, hum);
    } else {
        Serial.println("BME280 read error.");
    }

    if (imu.read(imuData)) {
        Serial.printf("Accel [g]   X: %.3f  Y: %.3f  Z: %.3f\n", imuData.ax, imuData.ay, imuData.az);
        Serial.printf("Gyro  [dps] X: %.2f  Y: %.2f  Z: %.2f\n", imuData.gx, imuData.gy, imuData.gz);
    } else {
        Serial.println("LSM6DSOX read error.");
    }

    delay(2000);
}
