#ifndef BOARD_HPP
#define BOARD_HPP

#include <Arduino.h>
#include <Wire.h>

extern const uint8_t pin_sda;
extern const uint8_t pin_scl;
extern const uint32_t baud;

typedef struct board_t {
    void (*init)(void);
} board_t;

extern board_t board;

#endif /* BOARD_HPP */
