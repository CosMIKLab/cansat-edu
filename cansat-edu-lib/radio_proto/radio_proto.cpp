#include "radio_proto.hpp"
#include <config.hpp>
#include <Arduino.h>
#include <SPI.h>

namespace radio_proto {

static SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

void select() {
    digitalWrite(PIN_RADIO_CS, LOW);
}

void deselect() {
    digitalWrite(PIN_RADIO_CS, HIGH);
}

void waitWhileBusy(uint16_t timeoutMs) {
    uint32_t deadline = millis() + timeoutMs;
    while (digitalRead(PIN_RADIO_BUSY) == HIGH) {
        if (millis() > deadline) return;
        delay(1);
    }
}

void reset() {
    pinMode(PIN_RADIO_RESET, OUTPUT);
    digitalWrite(PIN_RADIO_RESET, LOW);
    delay(10);
    digitalWrite(PIN_RADIO_RESET, HIGH);
    delay(20);
    waitWhileBusy(500);
}

void writeCommand(uint8_t opcode, const uint8_t* params, uint8_t len) {
    waitWhileBusy(100);
    SPI.beginTransaction(spiSettings);
    select();
    SPI.transfer(opcode);
    for (uint8_t i = 0; i < len; i++) SPI.transfer(params[i]);
    deselect();
    SPI.endTransaction();
}

void readCommand(uint8_t opcode, uint8_t* result, uint8_t len) {
    waitWhileBusy(100);
    SPI.beginTransaction(spiSettings);
    select();
    SPI.transfer(opcode);
    SPI.transfer(0x00);  // chip-status byte, discarded
    for (uint8_t i = 0; i < len; i++) result[i] = SPI.transfer(0x00);
    deselect();
    SPI.endTransaction();
}

void writeBuffer(uint8_t offset, const uint8_t* data, uint8_t len) {
    waitWhileBusy(100);
    SPI.beginTransaction(spiSettings);
    select();
    SPI.transfer(CMD_WRITE_BUFFER);
    SPI.transfer(offset);
    for (uint8_t i = 0; i < len; i++) SPI.transfer(data[i]);
    deselect();
    SPI.endTransaction();
}

void readBuffer(uint8_t offset, uint8_t* data, uint8_t len) {
    waitWhileBusy(100);
    SPI.beginTransaction(spiSettings);
    select();
    SPI.transfer(CMD_READ_BUFFER);
    SPI.transfer(offset);
    SPI.transfer(0x00);  // chip-status byte, discarded
    for (uint8_t i = 0; i < len; i++) data[i] = SPI.transfer(0x00);
    deselect();
    SPI.endTransaction();
}

uint8_t readStatusByte() {
    waitWhileBusy(100);
    SPI.beginTransaction(spiSettings);
    select();
    SPI.transfer(CMD_GET_STATUS);
    uint8_t status = SPI.transfer(0x00);
    deselect();
    SPI.endTransaction();
    return status;
}

uint16_t getIrqStatus() {
    uint8_t raw[2] = {};
    readCommand(CMD_GET_IRQ_STATUS, raw, sizeof(raw));
    return ((uint16_t)raw[0] << 8) | raw[1];
}

void clearIrqStatus(uint16_t mask) {
    uint8_t params[2] = { (uint8_t)(mask >> 8), (uint8_t)(mask & 0xFF) };
    writeCommand(CMD_CLEAR_IRQ_STATUS, params, sizeof(params));
}

} // namespace radio_proto
