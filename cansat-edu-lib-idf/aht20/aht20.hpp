#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

class AHT20 {
public:
    bool init(i2c_master_bus_handle_t bus);
    bool read(float& temp, float& hum);

private:
    i2c_master_dev_handle_t _dev = nullptr;
};
