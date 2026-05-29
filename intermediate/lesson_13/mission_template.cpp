#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// EMA szűrők
// TODO: Válassz alpha értékeket (javasolt: P=0.2, az=0.4)
static constexpr float ALPHA_P  = 0.2f;
static constexpr float ALPHA_AZ = 0.4f;
static float ema_p  = 1013.25f;
static float ema_az = 1.0f;

// 8-elemű csúszó átlag a hőmérséklethez
static constexpr uint8_t TBUF = 8;
static float tempBuf[TBUF] = {};
static uint8_t tIdx = 0;
static float tSum   = 0.0f;

// TODO: Implementáld az updateTempAvg() függvényt
// Gyűrű-puffer logika: kivon, beír, hozzáad, előre lép
float updateTempAvg(float sample) {
    return sample; // TODO
}

void setup() {
    board.init();
    // TODO: Inicializáld a szenzorokat, rádiót, SD-t
    // TODO: Inicializáld a szűrőket kezdeti mérésekkel
    // Hint: float t = bme.readTemperature();
    // Hint: float p = bme.readPressure() / 100.0F; ema_p = p;
    // Hint: sensors_event_t a, g, te; imu.getEvent(&a,&g,&te);
    //       ema_az = a.acceleration.z / 9.80665f;
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 200) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);

    float t_raw  = bme.readTemperature();
    float p_raw  = bme.readPressure() / 100.0F;
    float h_raw  = bme.readHumidity();
    float az_raw = accel.acceleration.z / 9.80665f;
    // TODO: Szűrők frissítése
    // TODO: Írd ki nyers és szűrt értékeket egymás mellé
    // TODO: Naplózd a szűrt értékeket SD-re
}
