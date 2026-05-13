#include <Arduino.h>
#include "radio.hpp"

uint8_t i = 0;

void setup() {
	Serial.begin(115200);
}

void loop() {
	radio.send();
	i++;
	delay(1000);
}
