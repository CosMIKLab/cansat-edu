#include <cansat.h>

void mission_setup() {
    Serial.println("Intelligens figyelőrendszer indul...");
}

void mission_loop() {
    float homerseklet = sensors.temperature();
    float az          = sensors.accel_z();

    if (homerseklet > 30.0) {
        Serial.println("FIGYELEM: Magas hőmérséklet!");
    } else {
        Serial.print("Hőmérséklet normális: ");
        Serial.print(homerseklet);
        Serial.println(" °C");
    }

    if (az < 0.5) {
        Serial.println("FIGYELEM: A CanSat erősen megdőlt!");
    } else {
        Serial.println("Helyzet: normális");
    }
}
