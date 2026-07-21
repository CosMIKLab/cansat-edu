#include "gnss.hpp"
#include <config.hpp>
#include <Arduino.h>
#include <Wire.h>
#include <string.h>

#define REG_BYTES_HIGH      0xFD
#define REG_DATA_STREAM     0xFF
#define MAX_READ_PER_SAMPLE 32

static bool    _present          = false;
static char    _streamWindow[192] = {0};
static uint8_t _streamLen        = 0;

static void appendStream(const uint8_t* data, uint8_t len) {
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

static void updateLatestNmea(GnssData& out) {
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

    // Consume everything through the sentence just reported so the same
    // line isn't returned again on the next poll.
    const char* rest = bestEnd + 1;
    size_t remaining = strlen(rest);
    memmove(_streamWindow, rest, remaining + 1);
    _streamLen = (uint8_t)remaining;
}

bool GNSS::init() {
    Wire.beginTransmission(GNSS_ADDR);
    _present = (Wire.endTransmission() == 0);
    return _present;
}

bool GNSS::read(GnssData& out) {
    out.present      = _present;
    out.pendingBytes = 0;
    out.nmea[0]      = '\0';
    if (!_present) return false;

    Wire.beginTransmission(GNSS_ADDR);
    Wire.write((uint8_t)REG_BYTES_HIGH);
    if (Wire.endTransmission(false) != 0) return false;
    Wire.requestFrom((uint8_t)GNSS_ADDR, (uint8_t)2);
    if (Wire.available() != 2) return false;

    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    uint16_t pending = ((uint16_t)hi << 8) | lo;
    out.pendingBytes = pending;

    if (pending == 0 || pending == 0xFFFF) {
        updateLatestNmea(out);
        return true;
    }

    uint8_t toRead = (pending < MAX_READ_PER_SAMPLE) ? (uint8_t)pending : MAX_READ_PER_SAMPLE;
    Wire.beginTransmission(GNSS_ADDR);
    Wire.write((uint8_t)REG_DATA_STREAM);
    if (Wire.endTransmission(false) != 0) return false;
    Wire.requestFrom((uint8_t)GNSS_ADDR, toRead);

    uint8_t raw[MAX_READ_PER_SAMPLE];
    uint8_t received = 0;
    while (Wire.available() && received < toRead) raw[received++] = Wire.read();

    appendStream(raw, received);
    updateLatestNmea(out);
    return true;
}

GNSS gnss;
