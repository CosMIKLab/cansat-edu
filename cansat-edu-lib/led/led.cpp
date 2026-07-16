#include "led.hpp"
#include <config.hpp>
#include <Arduino.h>
#include "esp32-hal-rmt.h"

// WS2816B: single NZR-protocol pixel, 48 bits total (16-bit G, 16-bit R,
// 16-bit B, MSB first, GRB channel order) — see led.hpp. Bit timing follows
// the same NZR encoding family as WS2812 (T0H=400ns/T0L=850ns,
// T1H=800ns/T1L=450ns); only the frame length (48 bits vs. 24) differs.

static rmt_obj_t* s_rmt = nullptr;

void StatusLed::begin() {
    s_rmt = rmtInit(PIN_STATUS_LED, RMT_TX_MODE, RMT_MEM_64);
    if (!s_rmt) {
        Serial.println("StatusLed: rmtInit failed");
        return;
    }
    rmtSetTick(s_rmt, 50.0f);  // 50 ns/tick (20 MHz-equivalent resolution)
    clear();
}

void StatusLed::set(uint8_t r, uint8_t g, uint8_t b) {
    if (!s_rmt) return;

    // Scale 8-bit -> 16-bit (0x00->0x0000, 0xFF->0xFFFF) by byte-duplication.
    uint16_t r16 = ((uint16_t)r << 8) | r;
    uint16_t g16 = ((uint16_t)g << 8) | g;
    uint16_t b16 = ((uint16_t)b << 8) | b;

    // GRB channel order, each channel MSB first -> 6 bytes total.
    uint8_t frame[6] = {
        (uint8_t)(g16 >> 8), (uint8_t)(g16 & 0xFF),
        (uint8_t)(r16 >> 8), (uint8_t)(r16 & 0xFF),
        (uint8_t)(b16 >> 8), (uint8_t)(b16 & 0xFF),
    };

    rmt_data_t bits[48];
    size_t idx = 0;
    for (uint8_t byteIdx = 0; byteIdx < 6; byteIdx++) {
        for (int8_t bitIdx = 7; bitIdx >= 0; bitIdx--) {
            bool one = (frame[byteIdx] >> bitIdx) & 0x01;
            if (one) {
                bits[idx].level0 = 1; bits[idx].duration0 = 16;  // 800 ns
                bits[idx].level1 = 0; bits[idx].duration1 = 9;   // 450 ns
            } else {
                bits[idx].level0 = 1; bits[idx].duration0 = 8;   // 400 ns
                bits[idx].level1 = 0; bits[idx].duration1 = 17;  // 850 ns
            }
            idx++;
        }
    }

    rmtWriteBlocking(s_rmt, bits, 48);
}

void StatusLed::clear() {
    set(0, 0, 0);
}

StatusLed led;
