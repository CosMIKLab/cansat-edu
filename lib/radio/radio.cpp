#include "radio.hpp"

uint8_t _init(void) {
    return 0;
}

uint8_t _send(void) {
	Serial.println("Hello World");
	return 0;
}

radio_t radio = {
    .settings = {
        "Hello World",
        "General Kenobi",
    },
    .init = _init,
	.send = _send,
};
