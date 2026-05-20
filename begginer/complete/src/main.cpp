#include <cansat.h>

// Students write these two functions in mission.cpp
void mission_setup();
void mission_loop();

static uint32_t _last = 0;

void setup() {
    board.init();
    sensors.begin();
    radio.begin();
    sd.begin();
    mission_setup();
}

void loop() {
    if (millis() - _last < 2000) return;
    _last = millis();
    mission_loop();
}
