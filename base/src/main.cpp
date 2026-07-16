#include <Arduino.h>
#include <string.h>
#include "board.hpp"
#include "bmp580.hpp"
#include "aht20.hpp"
#include "tmp102.hpp"
#include "lsm6ds3.hpp"
#include "gnss.hpp"
#include "led.hpp"
#include "radio.hpp"
#include "storage.hpp"

void setup() {
    board.init();
    led.begin();

    // Quick R/G/B self-test so a wiring/driver problem is visible immediately,
    // before sensor init messages scroll past on the serial monitor.
    Serial.println("LED self-test: RED");
    led.set(32, 0, 0); delay(300);
    Serial.println("LED self-test: GREEN");
    led.set(0, 32, 0); delay(300);
    Serial.println("LED self-test: BLUE");
    led.set(0, 0, 32); delay(300);
    Serial.println("LED self-test: OFF");
    led.clear();       delay(200);

    led.set(0, 0, 32);  // booting

    bool bmpOk = bmp580.init();
    Serial.println(bmpOk ? "BMP580 initialized." : "BMP580 init failed! Check wiring.");

    bool ahtOk = aht20.init();
    Serial.println(ahtOk ? "AHT20 initialized." : "AHT20 init failed! Check wiring.");

    bool tmpOk = tmp102.init();
    Serial.println(tmpOk ? "TMP102 initialized." : "TMP102 init failed! Check wiring.");

    bool imuOk = imu.init();
    Serial.println(imuOk ? "LSM6DS3 initialized." : "LSM6DS3 init failed! Check wiring.");

    bool gnssOk = gnss.init();
    Serial.println(gnssOk ? "GNSS initialized." : "GNSS not detected (optional).");

    bool radioOk = radio.init();
    Serial.println(radioOk ? "Radio initialized." : "Radio init failed!");

    if (!Storage::init(PIN_SD_CS)) {
        Serial.println("SD card init failed! Logging disabled.");
    } else {
        Serial.println("SD card ready.");
        Storage::event(bmpOk   ? "BMP580 OK"  : "BMP580 FAIL");
        Storage::event(ahtOk   ? "AHT20 OK"   : "AHT20 FAIL");
        Storage::event(tmpOk   ? "TMP102 OK"  : "TMP102 FAIL");
        Storage::event(imuOk   ? "LSM6DS3 OK" : "LSM6DS3 FAIL");
        Storage::event(gnssOk  ? "GNSS OK"    : "GNSS FAIL");
        Storage::event(radioOk ? "RADIO OK"   : "RADIO FAIL");
    }

    led.set(0, 32, 0);  // ready
}

static uint32_t lastRead = 0;
static constexpr uint32_t READ_INTERVAL_MS = 2000;

static uint32_t lastBlink = 0;
static uint8_t  blinkStep = 0;
static constexpr uint32_t BLINK_INTERVAL_MS = 1000;

void loop() {

    // Continuous LED color-cycle heartbeat — proves the driver can set more
    // than one color repeatedly (not just hold whatever "ready" left it at),
    // and that loop() itself is actually running, independent of sensors.
    // Max brightness + 1s per step, to rule out a perceptual threshold issue.
    if (millis() - lastBlink >= BLINK_INTERVAL_MS) {
        lastBlink = millis();
        switch (blinkStep) {
            case 0: led.set(255, 0, 0);   Serial.println("LED -> RED");   break;
            case 1: led.set(0, 255, 0);   Serial.println("LED -> GREEN"); break;
            case 2: led.set(0, 0, 255);   Serial.println("LED -> BLUE");  break;
            case 3: led.clear();          Serial.println("LED -> OFF");   break;
        }
        blinkStep = (blinkStep + 1) % 4;
    }

    if (millis() - lastRead < READ_INTERVAL_MS) return;
    lastRead = millis();

    TelemetryRecord rec = {};
    rec.time_ms = millis();

    if (bmp580.read(rec.temp, rec.pressure)) {
        Serial.printf("T: %.2f C  P: %.2f hPa\n", rec.temp, rec.pressure);
    } else {
        Serial.println("BMP580 read error.");
    }

    float ahtTemp = 0;
    if (aht20.read(ahtTemp, rec.humidity)) {
        Serial.printf("H: %.2f%%  (AHT20 temp: %.2f C)\n", rec.humidity, ahtTemp);
    } else {
        Serial.println("AHT20 read error.");
    }

    if (tmp102.read(rec.temp_secondary)) {
        Serial.printf("T2: %.2f C\n", rec.temp_secondary);
    } else {
        Serial.println("TMP102 read error.");
    }

    ImuData imuData;
    if (imu.read(imuData)) {
        rec.ax = imuData.ax; rec.ay = imuData.ay; rec.az = imuData.az;
        rec.gx = imuData.gx; rec.gy = imuData.gy; rec.gz = imuData.gz;
        Serial.printf("Accel [g]   X: %.3f  Y: %.3f  Z: %.3f\n", rec.ax, rec.ay, rec.az);
        Serial.printf("Gyro  [dps] X: %.2f  Y: %.2f  Z: %.2f\n", rec.gx, rec.gy, rec.gz);
    } else {
        Serial.println("LSM6DS3 read error.");
    }

    GnssData gnssData;
    if (gnss.read(gnssData) && gnssData.nmea[0] != '\0') {
        strncpy(rec.nmea, gnssData.nmea, sizeof(rec.nmea) - 1);
        rec.nmea[sizeof(rec.nmea) - 1] = '\0';
        Serial.printf("GNSS: %s\n", rec.nmea);
    }

    Storage::log(rec);

}
