#include <cansat.h>

void mission_setup() {
    Serial.println("Radio rendszer teszt...");
}

void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    radio.send(homerseklet, nyomas, paratartalom);

    Serial.println("Radioüzenet elküldve a földi állomásnak:");
    Serial.print("  "); Serial.print(homerseklet); Serial.print(" °C, ");
    Serial.print(nyomas);      Serial.print(" hPa, ");
    Serial.print(paratartalom);Serial.println(" %");
}
