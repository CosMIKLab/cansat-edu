#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"
#include "wifi.hpp"
#include <math.h>

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

// ---- Flight state machine ----
enum class FlightPhase { READY, ASCENT, DESCENT, LANDED };
static FlightPhase phase = FlightPhase::READY;

// ---- Calibration ----
static float groundPressure = 1013.25f;
static float minPressure    = 9999.0f;

// ---- EMA filters ----
static constexpr float ALPHA_P  = 0.25f;
static constexpr float ALPHA_AZ = 0.30f;
static float ema_p  = 1013.25f;
static float ema_az = 1.0f;

// ---- Multi-rate timers ----
static uint32_t tSensor = 0, tSD = 0, tRadio = 0, tSerial = 0;
static constexpr uint32_t DT_SENSOR =  100;
static constexpr uint32_t DT_SD     =  500;
static constexpr uint32_t DT_RADIO  = 2000;
static constexpr uint32_t DT_SERIAL = 1000;

static TelemetryRecord rec = {};

const char* phaseName(FlightPhase p) {
    switch (p) {
        case FlightPhase::READY:   return "READY";
        case FlightPhase::ASCENT:  return "ASCENT";
        case FlightPhase::DESCENT: return "DESCENT";
        case FlightPhase::LANDED:  return "LANDED";
        default: return "?";
    }
}

void updatePhase() {
    switch (phase) {
        case FlightPhase::READY:
            if (ema_az > 2.0f) {
                phase = FlightPhase::ASCENT;
                minPressure = ema_p;
                Storage::event("ASCENT");
                Serial.println("=== FELSZÁLLÁS ===");
                uint8_t m[] = {'L','A','U','N','C','H'};
                radio.send(m, 6);
            }
            break;

        case FlightPhase::ASCENT:
            if (ema_p < minPressure) minPressure = ema_p;
            if (ema_p > minPressure + 3.0f) {
                float alt = 44330.0f * (1.0f - powf(minPressure / groundPressure, 0.1903f));
                char buf[40];
                snprintf(buf, sizeof(buf), "APOGEE alt=%.1f", alt);
                Storage::event(buf);
                Serial.printf("=== TETŐPONT: %.1f m ===\n", alt);
                uint8_t m[] = {'A','P','O','G','E','E'};
                radio.send(m, 6);
                phase = FlightPhase::DESCENT;
                Storage::event("DESCENT");
            }
            break;

        case FlightPhase::DESCENT:
            if (ema_p > groundPressure - 10.0f && fabsf(ema_az - 1.0f) < 0.2f) {
                phase = FlightPhase::LANDED;
                Storage::event("LANDED");
                Serial.println("=== LANDOLT ===");
                uint8_t m[] = {'L','A','N','D','E','D'};
                radio.send(m, 6);
                Storage::close();
            }
            break;

        default: break;
    }
}

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    if (!radio.init())             { Serial.println("HIBA: Rádió");    while (1) delay(10); }
    if (!Storage::init(PIN_SD_CS)) Serial.println("FIGYELEM: SD nem elérhető");

    wifi.connect("IskolaHalozat", "jelszo123");

    groundPressure = bme.readPressure() / 100.0F;
    ema_p = groundPressure;
    minPressure = groundPressure;

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    ema_az = accel.acceleration.z / 9.80665f;

    Storage::event("BOOT_COMPLETE");
    Serial.printf("Rendszer kész — Talaj: %.2f hPa  WiFi: %s\n",
        groundPressure, wifi.connected() ? "OK" : "OFFLINE");
}

void loop() {
    uint32_t now = millis();

    // 10 Hz sensor acquisition + filter update
    if (now - tSensor >= DT_SENSOR) {
        tSensor = now;
        sensors_event_t accel, gyro, temp_e;
        imu.getEvent(&accel, &gyro, &temp_e);
        rec.time_ms  = now;
        rec.temp     = bme.readTemperature();
        rec.pressure = bme.readPressure() / 100.0F;
        rec.humidity = bme.readHumidity();
        rec.ax = accel.acceleration.x / 9.80665f;
        rec.ay = accel.acceleration.y / 9.80665f;
        rec.az = accel.acceleration.z / 9.80665f;
        rec.gx = gyro.gyro.x * (180.0f / M_PI);
        rec.gy = gyro.gyro.y * (180.0f / M_PI);
        rec.gz = gyro.gyro.z * (180.0f / M_PI);

        ema_p  = ALPHA_P  * rec.pressure + (1.0f - ALPHA_P)  * ema_p;
        ema_az = ALPHA_AZ * rec.az       + (1.0f - ALPHA_AZ) * ema_az;

        if (phase != FlightPhase::LANDED) updatePhase();
    }

    // 2 Hz SD log
    if (now - tSD >= DT_SD) {
        tSD = now;
        if (phase != FlightPhase::LANDED) Storage::log(rec);
    }

    // 0.5 Hz radio + WiFi
    if (now - tRadio >= DT_RADIO) {
        tRadio = now;
        int16_t t_r = (int16_t)(rec.temp * 100.0f);
        int16_t p_r = (int16_t)(rec.pressure * 10.0f);
        uint8_t payload[5] = {
            (uint8_t)(t_r>>8), (uint8_t)(t_r&0xFF),
            (uint8_t)(p_r>>8), (uint8_t)(p_r&0xFF),
            (uint8_t)phase
        };
        radio.send(payload, sizeof(payload));

        if (wifi.connected()) {
            wifi.send("http://192.168.1.5/telemetria", rec.temp, rec.pressure, rec.humidity);
        }
    }

    // 1 Hz serial status
    if (now - tSerial >= DT_SERIAL) {
        tSerial = now;
        Serial.printf("[%s] T=%.1f P=%.1f(%.1f) az=%.2f(%.2f)\n",
            phaseName(phase), rec.temp, rec.pressure, ema_p, rec.az, ema_az);
    }
}
