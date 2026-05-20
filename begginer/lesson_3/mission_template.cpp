#include <cansat.h>

void mission_setup() {
    Serial.println("Légköri szenzorok ellenőrzése...");
}

void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    Serial.println("--- Légköri adatok ---");
    Serial.print("Hőmérséklet:  "); Serial.print(homerseklet);  Serial.println(" °C");
    Serial.print("Légnyomás:    "); Serial.print(nyomas);        Serial.println(" hPa");
    Serial.print("Páratartalom: "); Serial.print(paratartalom);  Serial.println(" %");
}
