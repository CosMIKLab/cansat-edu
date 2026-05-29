#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// Exponential moving average (EMA) filters
static constexpr float ALPHA_P  = 0.2f;
static constexpr float ALPHA_AZ = 0.4f;
static float ema_p  = 1013.25f;
static float ema_az = 1.0f;

// Running average for temperature — 8-sample window
static constexpr uint8_t TBUF = 8;
static float tempBuf[TBUF] = {};
static uint8_t tIdx = 0;
static float tSum   = 0.0f;

float updateTempAvg(float sample) {
    tSum -= tempBuf[tIdx];
    tempBuf[tIdx] = sample;
    tSum += sample;
    tIdx = (tIdx + 1) % TBUF;
    return tSum / TBUF;
}

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    float t = bme.readTemperature();
    float p = bme.readPressure() / 100.0F;
    ema_p = p;
    for (int i = 0; i < TBUF; i++) tempBuf[i] = t;
    tSum = t * TBUF;

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    ema_az = accel.acceleration.z / 9.80665f;

    Serial.println("Szűrők inicializálva — EMA + csúszó átlag aktív");
    Serial.printf("  alpha_P=%.1f  alpha_az=%.1f  T_buf=%d minta\n",
        ALPHA_P, ALPHA_AZ, TBUF);
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 200) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    float t_raw = bme.readTemperature();
    float p_raw = bme.readPressure() / 100.0F;
    float h_raw = bme.readHumidity();
    float az_raw = accel.acceleration.z / 9.80665f;

    float t_avg = updateTempAvg(t_raw);
    ema_p  = ALPHA_P  * p_raw  + (1.0f - ALPHA_P)  * ema_p;
    ema_az = ALPHA_AZ * az_raw + (1.0f - ALPHA_AZ) * ema_az;

    Serial.printf("T raw=%.2f avg=%.2f  |  P raw=%.2f ema=%.2f  |  az raw=%.3f ema=%.3f\n",
        t_raw, t_avg, p_raw, ema_p, az_raw, ema_az);

    TelemetryRecord rec = {millis(), t_avg, ema_p, h_raw,
        accel.acceleration.x / 9.80665f,
        accel.acceleration.y / 9.80665f,
        ema_az,
        gyro.gyro.x * (180.0f / M_PI),
        gyro.gyro.y * (180.0f / M_PI),
        gyro.gyro.z * (180.0f / M_PI)};
    Storage::log(rec);
}
