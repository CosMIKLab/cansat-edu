#include "storage.hpp"
#include "config.hpp"
#include <Arduino.h>
#include <SD.h>
#include <cctype>
#include <stdio.h>

namespace Storage {

static bool     _ok         = false;
static File     _telem;
static File     _events;
static uint32_t _writeCount = 0;

// Scan root for highest S### directory and return next number (1-based).
static int nextSession() {
    File root = SD.open("/");
    int highest = 0;
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        if (entry.isDirectory()) {
            const char* name = entry.name();
            if (name[0] == 'S' &&
                isdigit((uint8_t)name[1]) &&
                isdigit((uint8_t)name[2]) &&
                isdigit((uint8_t)name[3]) &&
                name[4] == '\0') {
                int n = atoi(name + 1);
                if (n > highest) highest = n;
            }
        }
        entry.close();
    }
    root.close();
    return highest + 1;
}

bool init(uint8_t cs_pin) {
    if (!SD.begin(cs_pin)) return false;

    int n = nextSession();
    if (n > 999) return false;

    char session[5];
    snprintf(session, sizeof(session), "S%03d", n);

    char dir[6];
    snprintf(dir, sizeof(dir), "/%s", session);
    if (!SD.mkdir(dir)) return false;

    char path[22];
    snprintf(path, sizeof(path), "/%s/telem.csv", session);
    _telem = SD.open(path, FILE_WRITE);
    if (!_telem) return false;
    _telem.println("time_ms,temp_c,press_hpa,hum_pct,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps");
    _telem.flush();

    snprintf(path, sizeof(path), "/%s/events.txt", session);
    _events = SD.open(path, FILE_WRITE);
    if (!_events) { _telem.close(); return false; }

    _ok = true;
    _writeCount = 0;

    char msg[24];
    snprintf(msg, sizeof(msg), "BOOT  session=%s", session);
    event(msg);

    return true;
}

bool log(const TelemetryRecord& r) {
    if (!_ok || !_telem) return false;

    char buf[96];
    snprintf(buf, sizeof(buf),
        "%lu,%.2f,%.2f,%.2f,%.3f,%.3f,%.3f,%.2f,%.2f,%.2f",
        (unsigned long)r.time_ms,
        r.temp, r.pressure, r.humidity,
        r.ax, r.ay, r.az,
        r.gx, r.gy, r.gz);
    _telem.println(buf);

    if (++_writeCount % SD_FLUSH_EVERY == 0) _telem.flush();
    return true;
}

void event(const char* msg) {
    if (!_events) return;
    char buf[80];
    snprintf(buf, sizeof(buf), "[%8lu] %s", (unsigned long)millis(), msg);
    _events.println(buf);
    _events.flush();
}

void flush() {
    if (!_ok) return;
    if (_telem)  _telem.flush();
    if (_events) _events.flush();
}

void close() {
    if (!_ok) return;
    flush();
    _telem.close();
    _events.close();
    SD.end();
    _ok = false;
}

} // namespace Storage
