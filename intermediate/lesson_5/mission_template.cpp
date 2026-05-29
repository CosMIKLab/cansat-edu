#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_LSM6DSOX.h>
#include <math.h>

Adafruit_LSM6DSOX imu;

// TODO: Implementáld a dőlésszög-számító függvényt
// Hint: magnitude = sqrt(ax^2 + ay^2 + az^2)
//       angle = acos(az / magnitude) * 180 / PI
float tiltAngle(float ax, float ay, float az) {
    return 0.0f; // TODO
}

void setup() {
    board.init();
    // TODO: Inicializáld az IMU-t
    // Hint: imu.begin_I2C(LSM_ADDR);
}

void loop() {
    sensors_event_t accel, gyro, temp_e;
    // TODO: Olvasd le az IMU adatait
    // Hint: imu.getEvent(&accel, &gyro, &temp_e);
    // TODO: Számítsd ki a vektormagnitudót és a dőlésszöget
    // Hint: float ax = accel.acceleration.x / 9.80665f;
    // TODO: Írd ki és jelezd textesen (vízszintes / megdőlt)
    delay(500);
}
