#include "board.hpp"

void Board::init() {
    Serial.begin(BAUD_RADIO);
    delay(200);
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
}

Board board;
