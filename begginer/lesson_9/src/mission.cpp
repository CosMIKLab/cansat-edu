#include <cansat.h>

bool felszallt      = false;
bool tetopont_elert = false;
float min_nyomas    = 1013.25;

void mission_setup() {
    Serial.println("Repülési logika teszt...");
    min_nyomas = sensors.pressure();
    sd.note("Rendszer kesz a felszallasra");
}

void mission_loop() {
    float homerseklet = sensors.temperature();
    float nyomas      = sensors.pressure();
    float az          = sensors.accel_z();

    if (!felszallt && az > 2.0) {
        felszallt = true;
        sd.note("FELSZALLAS ESZLELVE");
        Serial.println("*** FELSZÁLLÁS ÉRZÉKELVE! ***");
    }

    if (felszallt && nyomas < min_nyomas) min_nyomas = nyomas;

    if (felszallt && !tetopont_elert && nyomas > min_nyomas + 2.0) {
        tetopont_elert = true;
        sd.note("TETOPONT ELERT");
        Serial.println("*** TETŐPONT ELÉRT! ***");
    }

    sd.log(homerseklet, nyomas, az);
    radio.send(homerseklet, nyomas, az);

    Serial.print("T="); Serial.print(homerseklet);
    Serial.print(" P="); Serial.print(nyomas);
    Serial.print(" az="); Serial.println(az);
}
