#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <math.h>

Adafruit_BME280 bme;
static float groundPressure = 1013.25f;

float pressureToAltitude(float pressure_hpa) {
    return 44330.0f * (1.0f - powf(pressure_hpa / groundPressure, 0.1903f));
}

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR)) {
        Serial.println("HIBA: BME280 nem elérhető!");
        while (1) delay(10);
    }
    groundPressure = bme.readPressure() / 100.0F;
    Serial.printf("Talaj-kalibráció: %.2f hPa (0 m)\n", groundPressure);
}

void loop() {
    float temp     = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = pressureToAltitude(pressure);

    Serial.printf("T: %.2f °C  P: %.2f hPa  Alt: %.1f m\n", temp, pressure, altitude);
    delay(2000);
}
