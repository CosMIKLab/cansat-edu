#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// TODO: Definiáld az állapotokat (enum class FlightPhase)
// Hint: INIT, READY, ASCENT, APOGEE, DESCENT, LANDED

// TODO: Deklarálj egy globális phase változót

// TODO: Implementáld a phaseName() segédfüggvényt

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat, rádiót, SD-t
    // TODO: Mérj talaj-nyomást: groundPressure = bme.readPressure() / 100.0F;
    // TODO: Állítsd be a kezdő fázist (READY)
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 500) return;
    lastCycle = millis();

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
    // TODO: SD napló

    // TODO: Implementáld az állapotgépet switch(phase) { case ...: ... }
    // READY  -> ASCENT  feltétel: az > 2.0g
    // ASCENT -> APOGEE  feltétel: nyomás minimum + 2 hPa emelkedés
    // APOGEE -> DESCENT automatikusan
    // DESCENT-> LANDED  feltétel: nyomás közel a talajhoz

    Serial.printf("[%s] P=%.1f az=%.2f\n", "?", rec.pressure, rec.az);
}
