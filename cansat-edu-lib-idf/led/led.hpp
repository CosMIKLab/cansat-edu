#pragma once
#include <stdint.h>

// WS2816B is a 16-bit-per-channel addressable LED (48 bits/pixel, GRB order)
// — NOT the same 8-bit/24-bit protocol as WS2812/WS2812B. set() takes 8-bit
// values for API convenience and scales them up to 16-bit internally.
// Driven via the RMT peripheral — unlike a plain GPIO LED, this requires a
// precisely timed serial bit stream, not a simple gpio_set_level() toggle.
class StatusLed {
public:
    bool init();
    void set(uint8_t r, uint8_t g, uint8_t b);
    void clear();
};
