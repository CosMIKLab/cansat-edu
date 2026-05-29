#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"
#include <math.h>

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

enum class Phase { READY, ASCENT, DESCENT };
static Phase phase = Phase::READY;

static float groundPressure = 1013.25f;
static float minPressure    = 9999.0f;
static float ema_pressure   = 0.0f;
static constexpr float EMA_ALPHA = 0.3f;

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    groundPressure = bme.readPressure() / 100.0F;
    ema_pressure   = groundPressure;
    minPressure    = groundPressure;

    Serial.printf("Talaj: %.2f hPa — Tetőpont-detektor kész\n", groundPressure);
    Storage::event("APOGEE_DETECTOR_ARMED");
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 500) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    float az = accel.acceleration.z / 9.80665f;
    float p  = bme.readPressure() / 100.0F;
    float t  = bme.readTemperature();
    float h  = bme.readHumidity();

    ema_pressure = EMA_ALPHA * p + (1.0f - EMA_ALPHA) * ema_pressure;

    TelemetryRecord rec = {millis(), t, p, h,
        accel.acceleration.x/9.80665f, accel.acceleration.y/9.80665f, az,
        gyro.gyro.x*(180.0f/M_PI), gyro.gyro.y*(180.0f/M_PI), gyro.gyro.z*(180.0f/M_PI)};
    Storage::log(rec);

    switch (phase) {
        case Phase::READY:
            if (az > 2.0f) {
                phase = Phase::ASCENT;
                minPressure = ema_pressure;
                Storage::event("ASCENT");
                Serial.println("FELSZÁLLÁS");
            }
            break;
        case Phase::ASCENT:
            if (ema_pressure < minPressure) minPressure = ema_pressure;
            Serial.printf("  EMA=%.2f  min=%.2f  delta=+%.2f\n",
                ema_pressure, minPressure, ema_pressure - minPressure);
            if (ema_pressure > minPressure + 3.0f) {
                phase = Phase::DESCENT;
                Storage::event("APOGEE_CONFIRMED");
                float alt = 44330.0f * (1.0f - powf(minPressure / groundPressure, 0.1903f));
                Serial.printf("=== TETŐPONT: %.1f m (P_min=%.2f hPa) ===\n", alt, minPressure);
                uint8_t msg[] = {'A','P','O','G','E','E'};
                radio.send(msg, sizeof(msg));
            }
            break;
        case Phase::DESCENT:
            Serial.printf("ERESZKEDÉS — EMA=%.2f hPa\n", ema_pressure);
            break;
    }
}
