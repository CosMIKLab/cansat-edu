#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// TODO: Definiáld az időzítő változókat és az intervallumokat
// static uint32_t tSensor = 0, tSD = 0, tRadio = 0, tSerial = 0;
// static constexpr uint32_t DT_SENSOR = 100, DT_SD = 500, DT_RADIO = 2000, DT_SERIAL = 1000;

static TelemetryRecord latest = {};

void setup() {
    board.init();
    // TODO: Inicializáld az összes modult
    Serial.println("Többsebességű ütemező: AKTIV");
}

void loop() {
    uint32_t now = millis();

    // TODO: 10 Hz szenzor olvasás (DT_SENSOR)
    // Hint: sensors_event_t accel, gyro, temp_e; imu.getEvent(&accel, &gyro, &temp_e);
    // Hint: latest.temp = bme.readTemperature();
    // Hint: latest.ax = accel.acceleration.x / 9.80665f;

    // TODO: 2 Hz SD naplózás (DT_SD)
    // TODO: 0.5 Hz rádió küldés (DT_RADIO)
    // TODO: 1 Hz Serial kiírás (DT_SERIAL)
    // FONTOS: egyetlen delay() sem lehet itt!
}
