#include "radio.hpp"

uint8_t _init(char* settings[SETTINGS_LENGTH]) {
	Serial1.begin(115200);
	for (uint8_t i = 0; i < SETTINGS_LENGTH; i++) {
	    Serial1.print("radio set");
	    Serial1.print(settings[i]);
	    Serial1.print("\r\n");
	}
    return 0;
}

uint8_t _send(void) {
	Serial.println("Hello World");
	return 0;
}

radio_t radio = {
    .settings = {
        "mod lora",
	    "freq 868100000",
	    "sf sf7",
	    "pa off",
	    "pwr 12"
    },
    .init = _init,
	.send = _send,
};
