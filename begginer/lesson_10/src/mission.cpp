#include <cansat.h>

bool felszallt      = false;
bool tetopont_elert = false;
float min_nyomas    = 1013.25;

void mission_setup() {
    wifi.connect("IskolaHalozat", "jelszo123");

    min_nyomas = sensors.pressure();

    sd.note("CSUA Kuldetes indítva");
    Serial.println("==============================");
    Serial.println("  CSÜA CanSat — KÉSZEN ÁLL  ");
    Serial.println("==============================");
}

void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();
    float az           = sensors.accel_z();

    sd.log(homerseklet, nyomas, paratartalom);
    radio.send(homerseklet, nyomas, paratartalom);

    if (wifi.connected()) {
        wifi.send("http://192.168.1.5/telemetria", homerseklet, nyomas, paratartalom);
    }

    if (!felszallt && az > 2.0) {
        felszallt = true;
        sd.note("FELSZALLAS");
        radio.send("FELSZALLAS!");
        Serial.println("*** FELSZÁLLÁS! ***");
    }

    if (felszallt && nyomas < min_nyomas) min_nyomas = nyomas;

    if (felszallt && !tetopont_elert && nyomas > min_nyomas + 2.0) {
        tetopont_elert = true;
        sd.note("TETOPONT");
        radio.send("TETOPONT!");
        Serial.println("*** TETŐPONT! ***");
    }

    Serial.print("T="); Serial.print(homerseklet);
    Serial.print("  P="); Serial.print(nyomas);
    Serial.print("  H="); Serial.print(paratartalom);
    Serial.print("  az="); Serial.println(az);
}
