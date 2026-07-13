#include "sd_easy.hpp"
#include <storage.hpp>
#include <config.hpp>

void SdEasy::begin() {
    bool ok = Storage::init(PIN_SD_CS);
    Serial.println(ok ? "SD: ready" : "SD: not found (continuing without SD)");
}

void SdEasy::log(float a, float b, float c) {
    TelemetryRecord r{};
    r.time_ms  = millis();
    r.temp     = a;
    r.pressure = b;
    r.humidity = c;
    Storage::log(r);
}

void SdEasy::note(const char* msg) {
    Storage::event(msg);
}

SdEasy sd;
