#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

static bool  launched       = false;
static float groundPressure = 1013.25f;

static float pressureHistory[5] = {};
static uint8_t histIdx = 0;

bool isPressureFalling() {
    float sum = 0.0f;
    for (int i = 0; i < 5; i++) sum += pressureHistory[i];
    return (sum / 5.0f) < (groundPressure - 1.5f);
}

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    groundPressure = bme.readPressure() / 100.0F;
    for (int i = 0; i < 5; i++) pressureHistory[i] = groundPressure;

    Serial.printf("Talaj referencia: %.2f hPa — Felszállás-detektor: FEGYVERZETT\n", groundPressure);
    Storage::event("LAUNCH_DETECTOR_ARMED");
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

    pressureHistory[histIdx] = p;
    histIdx = (histIdx + 1) % 5;

    if (!launched) {
        bool imuSpike        = (az > 2.5f);
        bool pressureFalling = isPressureFalling();

        Serial.printf("az=%.2f  P=%.1f  IMU:%s  dP:%s\n",
            az, p,
            imuSpike ? "SPIKE" : "normal",
            pressureFalling ? "ESIK" : "stabil");

        if (imuSpike && pressureFalling) {
            launched = true;
            Serial.println("=== FELSZÁLLÁS MEGERŐSÍTVE ===");
            Storage::event("LAUNCH_CONFIRMED");
            uint8_t msg[] = {'L','A','U','N','C','H'};
            radio.send(msg, sizeof(msg));
        }
    } else {
        Serial.printf("REPÜLÉS — T=%.1f P=%.1f az=%.2f\n", t, p, az);
    }
}
