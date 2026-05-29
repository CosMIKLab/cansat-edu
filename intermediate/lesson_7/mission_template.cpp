#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat, rádiót és SD kártyát
}

static uint32_t lastSend = 0;
static uint8_t txSeq = 0;

void loop() {
    if (millis() - lastSend < 2000) return;
    lastSend = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    TelemetryRecord rec = {};
    rec.time_ms  = millis();
    rec.temp     = bme.readTemperature();
    rec.pressure = bme.readPressure() / 100.0F;
    rec.humidity = bme.readHumidity();
    rec.ax = accel.acceleration.x / 9.80665f;
    rec.ay = accel.acceleration.y / 9.80665f;
    rec.az = accel.acceleration.z / 9.80665f;
    rec.gx = gyro.gyro.x * (180.0f / M_PI);
    rec.gy = gyro.gyro.y * (180.0f / M_PI);
    rec.gz = gyro.gyro.z * (180.0f / M_PI);

    // TODO: Építs egy bináris payload tömböt a hőmér. és légnyomásból
    // Hint: int16_t t_raw = (int16_t)(rec.temp * 100.0f);
    // Hint: radio.send(payload, sizeof(payload));
}
