#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR)) {
        Serial.println("HIBA: BME280 nem elérhető! Ellenőrizd a bekötést.");
        while (1) delay(10);
    }
    Serial.println("BME280 kalibrálva — légköri mérés indul");
}

void loop() {
    float temp     = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float humidity = bme.readHumidity();

    Serial.printf("T: %.2f °C  P: %.2f hPa  H: %.2f %%\n", temp, pressure, humidity);
    delay(2000);
}
