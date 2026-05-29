#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
    board.init();
    // TODO: Inicializáld a BME280 szenzort és ellenőrizd a visszatérési értéket
    // Hint: if (!bme.begin(BME280_ADDR)) { Serial.println("HIBA!"); while(1) delay(10); }
}

void loop() {
    // TODO: Olvasd le a szenzor értékeket
    // Hint: float temp = bme.readTemperature();
    // Hint: float press = bme.readPressure() / 100.0F;
    // Hint: float hum = bme.readHumidity();
    // TODO: Írd ki az értékeket a Serial monitorra
    delay(2000);
}
