#include "gnss.hpp"
#include "config.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "GNSS";

#define REG_BYTES_HIGH      0xFD
#define REG_DATA_STREAM     0xFF
#define MAX_READ_PER_SAMPLE 32

esp_err_t GNSS::_readReg(uint8_t reg, uint8_t* data, size_t len) {
    return i2c_master_transmit_receive(_dev, &reg, 1, data, len, 100);
}

bool GNSS::init(i2c_master_bus_handle_t bus) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = GNSS_ADDR,
        .scl_speed_hz    = 400000,
    };
    if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to I2C bus");
        return false;
    }

    // No chip-ID register on this part — a successful probe is our presence check.
    _present = (i2c_master_probe(bus, GNSS_ADDR, 100) == ESP_OK);
    if (!_present) {
        ESP_LOGW(TAG, "GNSS module not detected (optional hardware)");
    } else {
        ESP_LOGI(TAG, "Initialized OK");
    }
    return _present;
}

void GNSS::_appendStream(const uint8_t* data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        char c = (char)data[i];
        char printable = (c == '\r' || c == '\n' || (c >= 32 && c <= 126)) ? c : '.';
        if (_streamLen < sizeof(_streamWindow) - 1) {
            _streamWindow[_streamLen++] = printable;
        } else {
            memmove(_streamWindow, _streamWindow + 1, sizeof(_streamWindow) - 2);
            _streamWindow[sizeof(_streamWindow) - 2] = printable;
            _streamLen = sizeof(_streamWindow) - 1;
        }
        _streamWindow[_streamLen] = '\0';
    }
}

void GNSS::_updateLatestNmea(GnssData& out) const {
    const char* bestStart = nullptr;
    const char* bestEnd   = nullptr;

    for (const char* cursor = _streamWindow; *cursor != '\0'; cursor++) {
        if (*cursor != '$') continue;
        const char* end = strchr(cursor, '\n');
        if (end != nullptr) {
            bestStart = cursor;
            bestEnd   = end;
        }
    }

    if (bestStart == nullptr || bestEnd == nullptr || bestEnd <= bestStart) return;

    size_t len = (size_t)(bestEnd - bestStart);
    while (len > 0 && (bestStart[len - 1] == '\r' || bestStart[len - 1] == '\n')) len--;
    if (len > sizeof(out.nmea) - 1) len = sizeof(out.nmea) - 1;
    memcpy(out.nmea, bestStart, len);
    out.nmea[len] = '\0';
}

bool GNSS::read(GnssData& out) {
    out.present      = _present;
    out.pendingBytes = 0;
    out.nmea[0]      = '\0';
    if (!_present) return false;

    uint8_t pendingRaw[2];
    if (_readReg(REG_BYTES_HIGH, pendingRaw, sizeof(pendingRaw)) != ESP_OK) return false;

    uint16_t pending = ((uint16_t)pendingRaw[0] << 8) | pendingRaw[1];
    out.pendingBytes = pending;

    if (pending == 0 || pending == 0xFFFF) {
        _updateLatestNmea(out);
        return true;
    }

    uint8_t toRead = (pending < MAX_READ_PER_SAMPLE) ? (uint8_t)pending : MAX_READ_PER_SAMPLE;
    uint8_t raw[MAX_READ_PER_SAMPLE];
    if (_readReg(REG_DATA_STREAM, raw, toRead) != ESP_OK) return false;

    _appendStream(raw, toRead);
    _updateLatestNmea(out);
    return true;
}
