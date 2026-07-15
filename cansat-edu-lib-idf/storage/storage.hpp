#pragma once
#include <stdint.h>
#include "driver/spi_master.h"

struct TelemetryRecord {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
    float gx, gy, gz;
};

namespace Storage {
    // `host` must already be initialised with spi_bus_initialize() — the SD
    // card and the LoRa radio share this bus (SPI2) on separate CS lines.
    bool init(spi_host_device_t host);
    bool log(const TelemetryRecord& rec);
    void event(const char* msg);
    void flush();
    void close();
}
