#include <Arduino.h>
#include "board.hpp"

void setup() {
    board.init();

    Serial.println("==============================");
    Serial.println("  CSUA Muszaki Divizio        ");
    Serial.println("  Rendszer: AKTIV             ");
    Serial.println("==============================");
    Serial.println("Felkeszultseg: 100%");
}

void loop() {
    Serial.printf("[%8lu ms] Fedélzeti rendszer — normális\n", millis());
    delay(2000);
}
