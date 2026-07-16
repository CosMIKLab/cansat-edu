#include "board.hpp"

void Board::init() {
    Serial.begin(SERIAL_BAUD);
    delay(200);

    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(I2C_FREQ_HZ);

    // SPI2 is shared between the SD card and the radio; keep both chip-selects
    // idle-high before either device starts talking on the bus.
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);
    pinMode(PIN_RADIO_CS, OUTPUT);
    digitalWrite(PIN_RADIO_CS, HIGH);
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SD_CS);
}

Board board;
