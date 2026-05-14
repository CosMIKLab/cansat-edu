#include "board.hpp"

const uint8_t pin_sda = 4;
const uint8_t pin_scl = 5;
const uint32_t baud = 115200;

void _init_i2c(void){
    Wire.begin(pin_sda, pin_scl);
}

void _init_serial(void) {
    Serial.begin(baud);
}

void _init(void) {
    _init_i2c();
}

board_t board = {
    .init = _init,
};
