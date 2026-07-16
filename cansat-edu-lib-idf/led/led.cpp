#include "led.hpp"
#include "config.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"

static const char* TAG = "StatusLed";

// WS2816B: single NZR-protocol pixel, 48 bits total (16-bit G, 16-bit R,
// 16-bit B, MSB first, GRB channel order) — see led.hpp. Bit timing follows
// the same NZR encoding family as WS2812 (T0H=400ns/T0L=850ns,
// T1H=800ns/T1L=450ns); only the frame length (48 bits vs. 24) differs.
// Timing/frame-length confirmed against real hardware.

static rmt_channel_handle_t s_channel = nullptr;
static rmt_encoder_handle_t s_encoder = nullptr;

bool StatusLed::init() {
    rmt_tx_channel_config_t chan_cfg = {};
    chan_cfg.gpio_num          = PIN_LED;
    chan_cfg.clk_src           = RMT_CLK_SRC_DEFAULT;
    chan_cfg.resolution_hz     = 20000000;  // 20 MHz -> 0.05 us/tick
    chan_cfg.mem_block_symbols = 64;
    chan_cfg.trans_queue_depth = 4;

    if (rmt_new_tx_channel(&chan_cfg, &s_channel) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RMT TX channel");
        return false;
    }

    // bit0 = 400ns high + 850ns low = 8 + 17 ticks @ 20 MHz
    // bit1 = 800ns high + 450ns low = 16 + 9 ticks @ 20 MHz
    rmt_bytes_encoder_config_t enc_cfg = {};
    enc_cfg.bit0.duration0 = 8;  enc_cfg.bit0.level0 = 1;
    enc_cfg.bit0.duration1 = 17; enc_cfg.bit0.level1 = 0;
    enc_cfg.bit1.duration0 = 16; enc_cfg.bit1.level0 = 1;
    enc_cfg.bit1.duration1 = 9;  enc_cfg.bit1.level1 = 0;
    enc_cfg.flags.msb_first = 1;

    if (rmt_new_bytes_encoder(&enc_cfg, &s_encoder) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RMT bytes encoder");
        return false;
    }

    if (rmt_enable(s_channel) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable RMT channel");
        return false;
    }

    clear();
    ESP_LOGI(TAG, "Initialized OK");
    return true;
}

void StatusLed::set(uint8_t r, uint8_t g, uint8_t b) {
    if (!s_channel || !s_encoder) return;

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

    rmt_transmit_config_t tx_cfg = {};
    tx_cfg.loop_count = 0;
    rmt_transmit(s_channel, s_encoder, frame, sizeof(frame), &tx_cfg);
}

void StatusLed::clear() {
    set(0, 0, 0);
}
