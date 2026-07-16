#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <stdint.h>

struct TelemetryRecord {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float temp_secondary;  // TMP102
    float ax, ay, az;
    float gx, gy, gz;
    char  nmea[64];  // latest GNSS sentence, empty if none
};

namespace Storage {
    bool init(uint8_t cs_pin);
    bool log(const TelemetryRecord& rec);
    void event(const char* msg);
    void flush();
    void close();
}

#endif /* STORAGE_HPP */
