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

// TODO: Deklarálj egy 5-elemű nyomás-puffert a trend detektáláshoz
// static float pressureHistory[5] = {};

// TODO: Implementáld az isPressureFalling() függvényt
// Visszatér true-val, ha az átlag legalább 1.5 hPa-val alacsonyabb a talajnyomásnál
bool isPressureFalling() {
    return false; // TODO
}

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat, rádiót, SD-t
    // TODO: Mérj talaj-nyomást: groundPressure = bme.readPressure() / 100.0F;
    // TODO: Töltsd fel a pressureHistory-t a kezdeti nyomással
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 500) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    float az = accel.acceleration.z / 9.80665f;
    float p  = bme.readPressure() / 100.0F;
    // TODO: Puffer frissítés

    if (!launched) {
        // TODO: Ellenőrizd az IMU spike-ot (az > 2.5g) ÉS a nyomás csökkenést
        // TODO: Ha mindkettő teljesül: beállítás, esemény, rádió üzenet
    } else {
        Serial.printf("REPÜLÉS — P=%.1f\n", p);
    }
}
