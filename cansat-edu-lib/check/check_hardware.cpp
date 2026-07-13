#include <cansat.h>

static void result(const char* label, bool ok, float value, const char* unit) {
    Serial.print("  ");
    Serial.print(label);
    Serial.print(value, 2);
    Serial.print(" ");
    Serial.print(unit);
    Serial.println(ok ? "  [OK]" : "  [HIBA]");
}

void mission_setup() {
    Serial.println();
    Serial.println("=========================================");
    Serial.println("  MecsekSat Edu - Hardver ellenorzés   ");
    Serial.println("=========================================");
    Serial.println();
    // Framework already printed BME280/IMU/SD/Radio init status above this point.
    Serial.println("--- Szenzor leolvasás ---");

    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();
    float ax = sensors.accel_x();
    float ay = sensors.accel_y();
    float az = sensors.accel_z();

    bool bme_ok = (nyomas > 300.0f && nyomas < 1100.0f);
    result("Homerseklet:  ", (homerseklet > -40.0f && homerseklet < 85.0f && bme_ok), homerseklet, "*C");
    result("Legnyomas:    ", bme_ok, nyomas, "hPa");
    result("Paratartalom: ", (paratartalom >= 0.0f && paratartalom <= 100.0f && bme_ok), paratartalom, "%");

    float mag = sqrt(ax * ax + ay * ay + az * az);
    bool imu_ok = (mag > 0.5f && mag < 2.0f);
    result("IMU ero (mag):", imu_ok, mag, "g");

    Serial.println();
    Serial.println("(SD kartya és rádió státusz: lásd fent a boot üzenetekben)");
    Serial.println();

    if (bme_ok && imu_ok) {
        Serial.println("EREDMENY: Minden szenzor OK!");
    } else {
        Serial.println("EREDMENY: Hiba! Ellenorizd a kabeleket és az I2C cimet.");
        if (!bme_ok) Serial.println("  - BME280 nem válaszol (legnyomas = 0?)");
        if (!imu_ok) Serial.println("  - LSM6DSOX nem válaszol (IMU = 0 g?)");
    }
    Serial.println("=========================================");
    Serial.println("Kesz. Nyomj Ctrl+C-t, majd: ./cansat run");
}

void mission_loop() {}
