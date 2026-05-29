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
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    wifi.connect("IskolaHalozat", "jelszo123");
    if (wifi.connected()) {
        Serial.println("WiFi: csatlakozva — HTTP telemetria aktív");
    } else {
        Serial.println("WiFi: nem elérhető — offline módban folytatás");
    }
}

static uint32_t lastCycle = 0;
static uint8_t  txSeq     = 0;

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
    rec.ax = accel.acceleration.x / 9.80665f;
    rec.ay = accel.acceleration.y / 9.80665f;
    rec.az = accel.acceleration.z / 9.80665f;
    rec.gx = gyro.gyro.x * (180.0f / M_PI);
    rec.gy = gyro.gyro.y * (180.0f / M_PI);
    rec.gz = gyro.gyro.z * (180.0f / M_PI);

    Storage::log(rec);

    int16_t t_raw = (int16_t)(rec.temp * 100.0f);
    int16_t p_raw = (int16_t)(rec.pressure * 10.0f);
    uint8_t payload[9] = {
        (uint8_t)(t_raw>>8), (uint8_t)(t_raw&0xFF),
        (uint8_t)(p_raw>>8), (uint8_t)(p_raw&0xFF),
        (uint8_t)rec.humidity,
        (int8_t)(rec.ax*100.0f), (int8_t)(rec.ay*100.0f), (int8_t)(rec.az*100.0f),
        txSeq++
    };
    radio.send(payload, sizeof(payload));

    if (wifi.connected()) {
        wifi.send("http://192.168.1.5/telemetria", rec.temp, rec.pressure, rec.humidity);
    }

    Serial.printf("T=%.1f P=%.1f H=%.1f  WiFi:%s\n",
        rec.temp, rec.pressure, rec.humidity,
        wifi.connected() ? "OK" : "OFF");
}
