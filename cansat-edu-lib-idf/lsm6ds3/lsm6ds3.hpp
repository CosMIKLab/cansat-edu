#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

struct ImuData {
    float ax, ay, az;  // Accelerometer [g]
    float gx, gy, gz;  // Gyroscope [dps]
};

class LSM6DS3 {
public:
    bool init(i2c_master_bus_handle_t bus);
    bool read(ImuData& data);

private:
    i2c_master_dev_handle_t _dev    = nullptr;
    bool                    _ready  = false;
    uint8_t                 _address = 0;

    esp_err_t _readReg(uint8_t reg, uint8_t* data, size_t len);
    esp_err_t _writeReg(uint8_t reg, uint8_t value);
};
