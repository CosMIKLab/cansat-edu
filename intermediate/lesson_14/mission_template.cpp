#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"
#include "wifi.hpp"
#include <math.h>

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// ---- TODO: Állapotgép (FlightPhase enum) ----

// ---- TODO: EMA szűrők (ALPHA_P, ALPHA_AZ, ema_p, ema_az) ----

// ---- TODO: Kalibráció (groundPressure, minPressure) ----

// ---- TODO: Többsebességű időzítők (tSensor, tSD, tRadio, tSerial) ----

void updatePhase() {
    // TODO: Teljes repülési állapotgép implementálása
    // READY -> ASCENT: ema_az > 2.0
    // ASCENT -> DESCENT: ema_p > minPressure + 3.0 (APOGEE esemény)
    // DESCENT -> LANDED: nyomás közel talajhoz + stabil gyorsulás
}

void setup() {
    board.init();
    // TODO: Összes modul inicializálása
    // TODO: WiFi kapcsolódás
    // TODO: Talaj-kalibráció
    // Hint: groundPressure = bme.readPressure() / 100.0F;
    // Hint: sensors_event_t a, g, te; imu.getEvent(&a,&g,&te);
    //       ema_az = a.acceleration.z / 9.80665f;
    Storage::event("BOOT_COMPLETE");
}

void loop() {
    uint32_t now = millis();
    // TODO: 10 Hz szenzor olvasás + EMA frissítés + fázis frissítés
    // Hint: sensors_event_t accel, gyro, temp_e; imu.getEvent(&accel, &gyro, &temp_e);
    // Hint: rec.az = accel.acceleration.z / 9.80665f;
    // TODO: 2 Hz SD naplózás
    // TODO: 0.5 Hz rádió + WiFi küldés
    // TODO: 1 Hz Serial státusz
}
