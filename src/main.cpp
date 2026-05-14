#include <Arduino.h>
#include "sensors.hpp"
#include "board.hpp"
#include "radio.hpp"

void setup() {
    board.init();

if (bme.init() == 0) {
    Serial.println("BME280 Initialized successfully!");
  } else {
    Serial.println("BME280 Initialization failed! Check wiring.");
    while (1) delay(10);
  }

  if (imu.init() == 0) {
    Serial.println("LSM6DSOX Initialized successfully!");
  } else {
    Serial.println("LSM6DSOX Initialization failed! Check wiring.");
  }
}

float temp = 0.0;
float press = 0.0;
float hum = 0.0;
imu_data_t imu_data;

void loop() {

  if (bme.read_temp(&temp, &press, &hum) == 0) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" °C");
      Serial.print("Pressure: ");
      Serial.print(press);
      Serial.println(" hPa");
      Serial.print("Humidity: ");
      Serial.print(hum);
      Serial.println(" % RH");
  } else {
      Serial.println("Error reading temperature.");
  }

  if (imu.read(&imu_data) == 0) {
      Serial.print("Accel (g)   [X, Y, Z]: ");
      Serial.print(imu_data.ax, 3); Serial.print(", ");
      Serial.print(imu_data.ay, 3); Serial.print(", ");
      Serial.println(imu_data.az, 3);

      Serial.print("Gyro  (dps) [X, Y, Z]: ");
      Serial.print(imu_data.gx, 2); Serial.print(", ");
      Serial.print(imu_data.gy, 2); Serial.print(", ");
      Serial.println(imu_data.gz, 2);
  } else {
      Serial.println("Error reading LSM6DSOX.");
  }

  delay(2000);
}
