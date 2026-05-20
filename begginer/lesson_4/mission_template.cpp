#include <cansat.h>

void mission_setup() {
    Serial.println("Mozgásérzékelő teszt indul...");
}

void mission_loop() {
    float ax = sensors.accel_x();
    float ay = sensors.accel_y();
    float az = sensors.accel_z();

    Serial.println("--- Gyorsulásmérő ---");
    Serial.print("X: "); Serial.print(ax); Serial.println(" g");
    Serial.print("Y: "); Serial.print(ay); Serial.println(" g");
    Serial.print("Z: "); Serial.print(az); Serial.println(" g");
    Serial.println("(Z = 1.0 amikor a CanSat vízszintes)");
}
