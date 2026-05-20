#include <cansat.h>

void mission_setup() {
    Serial.println("==============================");
    Serial.println("  CSÜA Küldetésirányítás     ");
    Serial.println("  Minden rendszer elindult!  ");
    Serial.println("==============================");
}

void mission_loop() {
    Serial.println("CanSat aktív — rendszerek OK");
}
