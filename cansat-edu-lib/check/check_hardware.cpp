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
    // Framework already printed BMP580/AHT20/TMP102/IMU/GNSS/SD/Radio init status above this point.
    Serial.println("--- Szenzor leolvasás ---");

    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();
    float homerseklet2 = sensors.temperature_secondary();
    float ax = sensors.accel_x();
    float ay = sensors.accel_y();
    float az = sensors.accel_z();

    bool bmp_ok = (nyomas > 300.0f && nyomas < 1100.0f);
    bool aht_ok = sensors.aht20_present() && (paratartalom >= 0.0f && paratartalom <= 100.0f);
    result("Homerseklet:  ", (homerseklet > -40.0f && homerseklet < 85.0f && bmp_ok), homerseklet, "*C");
    result("Legnyomas:    ", bmp_ok, nyomas, "hPa");
    result("Paratartalom: ", aht_ok, paratartalom, "%");
    result("Homerseklet2: ", sensors.tmp102_present() && (homerseklet2 > -40.0f && homerseklet2 < 85.0f), homerseklet2, "*C");

    float mag = sqrt(ax * ax + ay * ay + az * az);
    bool imu_ok = (mag > 0.5f && mag < 2.0f);
    result("IMU ero (mag):", imu_ok, mag, "g");

    bool gnss_ok = sensors.gnss_available();
    Serial.print("  GNSS modul:   ");
    Serial.println(gnss_ok ? "eszlelve  [OK]" : "nem eszlelheto  [opcionalis]");

    Serial.println();
    Serial.println("(SD kartya, rádió és LED státusz: lásd fent a boot üzenetekben)");
    Serial.println();

    if (bmp_ok && aht_ok && imu_ok) {
        Serial.println("EREDMENY: Minden szenzor OK!");
    } else {
        Serial.println("EREDMENY: Hiba! Ellenorizd a kabeleket és az I2C cimet.");
        if (!bmp_ok) Serial.println("  - BMP580 nem válaszol (legnyomas = 0?)");
        if (!aht_ok) Serial.println("  - AHT20 nem válaszol (paratartalom = 0?)");
        if (!imu_ok) Serial.println("  - LSM6DS3 nem válaszol (IMU = 0 g?)");
    }
    Serial.println("=========================================");
    Serial.println("Kesz. Nyomj Ctrl+C-t, majd: ./cansat run");
}

void mission_loop() {}
