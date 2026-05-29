#pragma once
#include <stdint.h>

struct TelemetryRecord {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
    float gx, gy, gz;
};

namespace Storage {
    bool init();
    bool log(const TelemetryRecord& rec);
    void event(const char* msg);
    void flush();
    void close();
}
