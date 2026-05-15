#include "board.hpp"

void Board::init() {
    Serial.begin(BAUD_USB);
    Wire.begin(PIN_SDA, PIN_SCL);
}

Board board;
