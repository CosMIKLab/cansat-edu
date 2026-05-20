#include <cansat.h>

void mission_setup() {
    Serial.println("SD kártya adatnaplózás teszt...");
    sd.note("Rendszer elindult!");
}

void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    sd.log(homerseklet, nyomas, paratartalom);
    sd.note("Meres elvegezve");

    Serial.print("Mentve: ");
    Serial.print(homerseklet); Serial.print(" °C, ");
    Serial.print(nyomas);      Serial.print(" hPa, ");
    Serial.print(paratartalom);Serial.println(" %");
}
