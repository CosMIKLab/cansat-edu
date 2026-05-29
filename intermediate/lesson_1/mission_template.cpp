#include <Arduino.h>
#include "board.hpp"

void setup() {
    board.init();

    // TODO: Írd meg a startup üzenetet a Serial monitorra
    // Pl.: Serial.println("Rendszer aktív");
}

void loop() {
    // TODO: Írj ki egy állapotüzenetet másodpercenként
    // Pl.: Serial.printf("[%lu ms] ...\n", millis());
    delay(2000);
}
