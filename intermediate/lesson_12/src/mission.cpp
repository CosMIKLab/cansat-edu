#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// Non-blocking multi-rate scheduler
static uint32_t tSensor = 0;
static uint32_t tRadio  = 0;
static uint32_t tSD     = 0;
static uint32_t tSerial = 0;

static constexpr uint32_t DT_SENSOR =  100;   // 10 Hz
static constexpr uint32_t DT_SD     =  500;   //  2 Hz
static constexpr uint32_t DT_RADIO  = 2000;   // 0.5 Hz
static constexpr uint32_t DT_SERIAL = 1000;   //  1 Hz

static TelemetryRecord latest = {};

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    Serial.println("Többsebességű ütemező: AKTIV");
    Serial.printf("  Szenzor: %ums  SD: %ums  Rádió: %ums  Serial: %ums\n",
        DT_SENSOR, DT_SD, DT_RADIO, DT_SERIAL);
}

void loop() {
    uint32_t now = millis();

    // 10 Hz — sensor acquisition
    if (now - tSensor >= DT_SENSOR) {
        tSensor = now;
        sensors_event_t accel, gyro, temp_e;
        imu.getEvent(&accel, &gyro, &temp_e);
        latest.time_ms  = now;
        latest.temp     = bme.readTemperature();
        latest.pressure = bme.readPressure() / 100.0F;
        latest.humidity = bme.readHumidity();
        latest.ax = accel.acceleration.x / 9.80665f;
        latest.ay = accel.acceleration.y / 9.80665f;
        latest.az = accel.acceleration.z / 9.80665f;
        latest.gx = gyro.gyro.x * (180.0f / M_PI);
        latest.gy = gyro.gyro.y * (180.0f / M_PI);
        latest.gz = gyro.gyro.z * (180.0f / M_PI);
    }

    // 2 Hz — SD card log
    if (now - tSD >= DT_SD) {
        tSD = now;
        Storage::log(latest);
    }

    // 0.5 Hz — radio burst
    if (now - tRadio >= DT_RADIO) {
        tRadio = now;
        int16_t t_r = (int16_t)(latest.temp * 100.0f);
        int16_t p_r = (int16_t)(latest.pressure * 10.0f);
        uint8_t payload[4] = {
            (uint8_t)(t_r>>8), (uint8_t)(t_r&0xFF),
            (uint8_t)(p_r>>8), (uint8_t)(p_r&0xFF)
        };
        radio.send(payload, sizeof(payload));
    }

    // 1 Hz — human-readable serial output
    if (now - tSerial >= DT_SERIAL) {
        tSerial = now;
        Serial.printf("[%lu ms] T=%.1f P=%.1f az=%.2f\n",
            latest.time_ms, latest.temp, latest.pressure, latest.az);
    }
}
