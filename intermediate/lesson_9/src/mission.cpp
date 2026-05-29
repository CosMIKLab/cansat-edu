#include <Arduino.h>
#include "board.hpp"
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSOX.h>
#include "storage.hpp"
#include "radio.hpp"

Adafruit_BME280  bme;
Adafruit_LSM6DSOX imu;

enum class FlightPhase { INIT, READY, ASCENT, APOGEE, DESCENT, LANDED };
static FlightPhase phase = FlightPhase::INIT;

static float groundPressure = 1013.25f;
static float minPressure    = 1013.25f;

const char* phaseName(FlightPhase p) {
    switch (p) {
        case FlightPhase::INIT:    return "INIT";
        case FlightPhase::READY:   return "READY";
        case FlightPhase::ASCENT:  return "ASCENT";
        case FlightPhase::APOGEE:  return "APOGEE";
        case FlightPhase::DESCENT: return "DESCENT";
        case FlightPhase::LANDED:  return "LANDED";
        default: return "UNKNOWN";
    }
}

void setup() {
    board.init();
    if (!bme.begin(BME280_ADDR))  { Serial.println("HIBA: BME280");   while (1) delay(10); }
    if (!imu.begin_I2C(LSM_ADDR)) { Serial.println("HIBA: LSM6DSOX"); while (1) delay(10); }
    radio.init();
    Storage::init(PIN_SD_CS);

    groundPressure = bme.readPressure() / 100.0F;
    minPressure    = groundPressure;
    phase = FlightPhase::READY;
    Storage::event("READY");
    Serial.printf("Állapotgép: %s | Talaj: %.2f hPa\n", phaseName(phase), groundPressure);
}

static uint32_t lastCycle = 0;

void loop() {
    if (millis() - lastCycle < 500) return;
    lastCycle = millis();

    sensors_event_t accel, gyro, temp_e;
    imu.getEvent(&accel, &gyro, &temp_e);
    float az = accel.acceleration.z / 9.80665f;

    TelemetryRecord rec = {};
    rec.time_ms  = millis();
    rec.temp     = bme.readTemperature();
    rec.pressure = bme.readPressure() / 100.0F;
    rec.humidity = bme.readHumidity();
    rec.ax = accel.acceleration.x / 9.80665f;
    rec.ay = accel.acceleration.y / 9.80665f;
    rec.az = az;
    rec.gx = gyro.gyro.x * (180.0f / M_PI);
    rec.gy = gyro.gyro.y * (180.0f / M_PI);
    rec.gz = gyro.gyro.z * (180.0f / M_PI);
    Storage::log(rec);

    switch (phase) {
        case FlightPhase::READY:
            if (az > 2.0f) {
                phase = FlightPhase::ASCENT;
                Storage::event("ASCENT");
                Serial.println("*** FELSZÁLLÁS ***");
            }
            break;
        case FlightPhase::ASCENT:
            if (rec.pressure < minPressure) minPressure = rec.pressure;
            if (rec.pressure > minPressure + 2.0f) {
                phase = FlightPhase::APOGEE;
                Storage::event("APOGEE");
                Serial.printf("*** TETŐPONT: %.2f hPa ***\n", minPressure);
            }
            break;
        case FlightPhase::APOGEE:
            phase = FlightPhase::DESCENT;
            Storage::event("DESCENT");
            break;
        case FlightPhase::DESCENT:
            if (rec.pressure > groundPressure - 5.0f && az < 1.2f) {
                phase = FlightPhase::LANDED;
                Storage::event("LANDED");
                Serial.println("*** LANDOLT ***");
            }
            break;
        default: break;
    }

    Serial.printf("[%s] T=%.1f P=%.1f az=%.2f\n",
        phaseName(phase), rec.temp, rec.pressure, az);
}
