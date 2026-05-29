#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <math.h>

Adafruit_BME280 bme;

static float groundPressure = 1013.25f;

// TODO: Implementáld a magasságszámító függvényt
// Hint: altitude = 44330 * (1 - (P/P0)^0.1903)
float pressureToAltitude(float pressure_hpa) {
    return 0.0f; // TODO
}

void setup() {
    board.init();
    // TODO: Inicializáld a BME280-t
    // TODO: Mérj egy talaj-nyomást és mentsd el groundPressure-be
    // Hint: groundPressure = bme.readPressure() / 100.0F;
}

void loop() {
    // TODO: Olvasd le a nyomást és számítsd ki a magasságot
    // Hint: float press = bme.readPressure() / 100.0F;
    delay(2000);
}
