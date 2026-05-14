#ifndef RADIO_HPP
#define RADIO_HPP

#include <Arduino.h>

#define SETTINGS_LENGTH 5

typedef struct radio_t {
    const char* settings[SETTINGS_LENGTH];
    uint8_t (*init)(char* settings[SETTINGS_LENGTH]);
	uint8_t (*send)(void);
} radio_t;

extern radio_t radio;

#endif /* RADIO_HPP */
