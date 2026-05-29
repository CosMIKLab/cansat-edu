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
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    if (!radio.init())             { Serial.println("HIBA: Rádió");    while (1) delay(10); }
    Storage::init(PIN_SD_CS);
    Serial.println("LoRa rádió konfigurálva — 868.1 MHz SF7");
}

static uint32_t lastSend = 0;
static uint8_t  txSeq    = 0;

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
    Storage::log(rec);

    // 9-byte payload: [temp_i16, press_i16, hum_u8, ax_i8, ay_i8, az_i8, seq_u8]
    int16_t t_raw = (int16_t)(rec.temp     * 100.0f);
    int16_t p_raw = (int16_t)(rec.pressure *  10.0f);
    uint8_t payload[9];
    payload[0] = t_raw >> 8;               payload[1] = t_raw & 0xFF;
    payload[2] = p_raw >> 8;               payload[3] = p_raw & 0xFF;
    payload[4] = (uint8_t)rec.humidity;
    payload[5] = (int8_t)(rec.ax * 100.0f);
    payload[6] = (int8_t)(rec.ay * 100.0f);
    payload[7] = (int8_t)(rec.az * 100.0f);
    payload[8] = txSeq++;

    radio.send(payload, sizeof(payload));
    Serial.printf("TX #%d  T=%.1f P=%.1f\n", txSeq - 1, rec.temp, rec.pressure);
}
