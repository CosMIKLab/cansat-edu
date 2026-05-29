#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_LSM6DSOX.h>
#include <math.h>

Adafruit_LSM6DSOX imu;

float tiltAngle(float ax, float ay, float az) {
    float magnitude = sqrtf(ax*ax + ay*ay + az*az);
    if (magnitude < 0.01f) return 0.0f;
    float cos_z = az / magnitude;
    if (cos_z >  1.0f) cos_z =  1.0f;
    if (cos_z < -1.0f) cos_z = -1.0f;
    return acosf(cos_z) * 180.0f / M_PI;
}

void setup() {
    board.init();
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    Serial.println("Vektor-analitika: AKTIV");
}

void loop() {
    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    float ax = accel.acceleration.x / 9.80665f;
    float ay = accel.acceleration.y / 9.80665f;
    float az = accel.acceleration.z / 9.80665f;

    float mag   = sqrtf(ax*ax + ay*ay + az*az);
    float angle = tiltAngle(ax, ay, az);

    Serial.printf("|a| = %.3f g   Dőlés: %.1f fok", mag, angle);
    if (angle < 15.0f)      Serial.println("  [VÍZSZINTES]");
    else if (angle < 45.0f) Serial.println("  [MEGDŐLT]");
    else                    Serial.println("  [ERŐSEN MEGDŐLT]");

    delay(500);
}
