#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"
#include "wifi.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    // TODO: Inicializáld az összes modult
    // TODO: Csatlakozz WiFi-hez: wifi.connect("SSID", "jelszo")
    // TODO: Ellenőrizd: wifi.connected()
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 2000) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    TelemetryRecord rec = {};
    rec.time_ms  = millis();
    rec.temp     = bme.readTemperature();
    rec.pressure = bme.readPressure() / 100.0F;
    rec.humidity = bme.readHumidity();
    // TODO: Töltsd fel a rec accel/gyro mezőit
    // TODO: SD napló, rádió küldés

    // TODO: Ha WiFi elérhető, küldd el HTTP GET-tel
    // Hint: wifi.send("http://IP/endpoint", rec.temp, rec.pressure, rec.humidity);
}
