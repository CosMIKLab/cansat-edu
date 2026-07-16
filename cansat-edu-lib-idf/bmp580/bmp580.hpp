#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

class BMP580 {
public:
    bool init(i2c_master_bus_handle_t bus);
    bool read(float& temp, float& press);

private:
    i2c_master_dev_handle_t _dev     = nullptr;
    bool                    _present = false;

    esp_err_t _readReg(uint8_t reg, uint8_t* data, size_t len);
    esp_err_t _writeReg(uint8_t reg, uint8_t value);
    void      _setPowerMode(uint8_t mode);
};
