#pragma once
#include <stdint.h>
#include "driver/spi_master.h"

// E22-900M22S (SX1262 core) LoRa radio driver, SPI-based. Shares its SPI
// bus (SPI2) with the SD card (Lesson 09) on a separate CS line.
class Radio {
public:
    // `host` must already be initialised with spi_bus_initialize().
    bool init(spi_host_device_t host);
    bool present() const;
    bool send(const uint8_t* payload, uint8_t len);
    int  receive(uint8_t* buffer, uint8_t maxLen, uint32_t timeoutMs);

private:
    spi_device_handle_t _dev   = nullptr;
    bool                _ready = false;

    void waitWhileBusy(uint16_t timeoutMs);
    void reset();
    void writeCommand(uint8_t opcode, const uint8_t* params, uint8_t len);
    void readCommand(uint8_t opcode, uint8_t* result, uint8_t len);
    void writeBuffer(uint8_t offset, const uint8_t* data, uint8_t len);
    void readBuffer(uint8_t offset, uint8_t* data, uint8_t len);
    uint8_t  readStatusByte();
    uint16_t getIrqStatus();
    void     clearIrqStatus(uint16_t mask);
    void     configureModem();
};
