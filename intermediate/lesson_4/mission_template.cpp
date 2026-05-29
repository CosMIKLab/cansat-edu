#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    // TODO: Inicializáld a BME280-t és az LSM6DSOX IMU-t
    // Hint: bme.begin(BME280_ADDR); imu.begin_I2C(LSM_ADDR);
}

void loop() {
    // TODO: Olvasd le a BME280 adatait
    // Hint: float temp = bme.readTemperature();

    sensors_event_t accel, gyro, temp_e;
    // TODO: Olvasd le az IMU adatait
    // Hint: imu.getEvent(&accel, &gyro, &temp_e);
    // Hint: float ax = accel.acceleration.x / 9.80665f;  // m/s² -> g
    // Hint: float gx = gyro.gyro.x * (180.0f / M_PI);    // rad/s -> dps
    // TODO: Írd ki az accel és gyro értékeket
    delay(2000);
}
