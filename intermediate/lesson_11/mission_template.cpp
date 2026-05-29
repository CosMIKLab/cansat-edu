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
// TODO: Válassz egy alpha értéket az EMA szűrőhöz (0.1 - 0.5 ajánlott)
static constexpr float EMA_ALPHA = 0.3f;

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat, rádiót, SD-t
    // TODO: Mérj talaj-nyomást, inicializáld az ema_pressure-t
    // Hint: groundPressure = bme.readPressure() / 100.0F;
    // Hint: ema_pressure = groundPressure;
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 500) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    float az = accel.acceleration.z / 9.80665f;
    float p  = bme.readPressure() / 100.0F;
    // TODO: Szenzor olvasás (temp, humidity)

    // TODO: Frissítsd az EMA szűrőt: ema = alpha*p + (1-alpha)*ema

    switch (phase) {
        case Phase::READY:
            // TODO: Felszállás detektálás (az > 2g)
            break;

        case Phase::ASCENT:
            // TODO: Nyomás minimum követés
            // TODO: 3 hPa hisztérezis-feltétel a tetőpont megerősítéséhez
            // TODO: Magasság kiszámítása és kiírása
            break;

        case Phase::DESCENT:
            Serial.printf("ERESZKEDÉS — EMA=%.2f hPa\n", ema_pressure);
            break;
    }
}
