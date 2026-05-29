#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }

    if (!Storage::init(PIN_SD_CS)) {
        Serial.println("FIGYELEM: SD kártya nem elérhető — naplózás letiltva");
    } else {
        Serial.println("SD kártya: KÉSZ — telem.csv létrehozva");
        Storage::event("BOOT_OK");
    }
}

static uint32_t lastLog = 0;

void loop() {
    if (millis() - lastLog < 2000) return;
    lastLog = millis();

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

    Storage::log(rec);

    Serial.printf("[%lu] T=%.1f P=%.1f H=%.1f  ax=%.2f ay=%.2f az=%.2f\n",
        rec.time_ms, rec.temp, rec.pressure, rec.humidity, rec.ax, rec.ay, rec.az);
}
