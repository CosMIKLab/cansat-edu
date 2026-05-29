#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat
    // TODO: Inicializáld a Storage modult (Storage::init(PIN_SD_CS))
    // TODO: Írj egy eseményt az events.txt-be (Storage::event("BOOT_OK"))
}

static uint32_t lastLog = 0;

void loop() {
    if (millis() - lastLog < 2000) return;
    lastLog = millis();

    sensors_event_t accel, gyro, temp_e;
    // TODO: Olvasd le az IMU adatait: imu.getEvent(&accel, &gyro, &temp_e);

    TelemetryRecord rec = {};
    rec.time_ms = millis();
    // TODO: Töltsd fel a rec struktúrát szenzor adatokkal
    // Hint: rec.temp     = bme.readTemperature();
    // Hint: rec.pressure = bme.readPressure() / 100.0F;
    // Hint: rec.ax = accel.acceleration.x / 9.80665f;
    // Hint: rec.gx = gyro.gyro.x * (180.0f / M_PI);
    // TODO: Naplózd az adatokat SD kártyára (Storage::log(rec))
    // TODO: Írd ki a konzolra is
}
