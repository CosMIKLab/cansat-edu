#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))      { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR))     { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    Serial.println("Szenzor-csomag inicializálva");
}

void loop() {
    float temp     = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    float ax = accel.acceleration.x / 9.80665f;
    float ay = accel.acceleration.y / 9.80665f;
    float az = accel.acceleration.z / 9.80665f;
    float gx = gyro.gyro.x * (180.0f / M_PI);
    float gy = gyro.gyro.y * (180.0f / M_PI);
    float gz = gyro.gyro.z * (180.0f / M_PI);

    Serial.printf("T: %.1f C  P: %.1f hPa\n", temp, pressure);
    Serial.printf("Accel [g]  X: %.3f  Y: %.3f  Z: %.3f\n", ax, ay, az);
    Serial.printf("Gyro [dps] X: %.1f  Y: %.1f  Z: %.1f\n", gx, gy, gz);
    Serial.println("---");
    delay(2000);
}
