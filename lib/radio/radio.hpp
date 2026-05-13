#include <Arduino.h>

typedef struct radio_t {
	uint8_t (*send)(void);
} radio_t;

extern radio_t radio;
