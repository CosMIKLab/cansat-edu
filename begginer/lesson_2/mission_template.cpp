#include <cansat.h>

void mission_setup() {
    Serial.println("Hőmérő szenzor inicializálva.");
}

void mission_loop() {
    float homerseklet = sensors.temperature();

    Serial.print("Hőmérséklet: ");
    Serial.print(homerseklet);
    Serial.println(" °C");
}
