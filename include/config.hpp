#pragma once
#include <stdint.h>

// I2C bus
static constexpr uint8_t  PIN_SDA = 4;
static constexpr uint8_t  PIN_SCL = 5;

// UART baud rate (shared — hardware switch selects USB or radio)
static constexpr uint32_t BAUD_RADIO = 115200;

// I2C addresses
static constexpr uint8_t  BME280_ADDR = 0x76;
static constexpr uint8_t  LSM_ADDR    = 0x6A;

// IMU sensitivity: ±2 g / ±250 dps at 104 Hz ODR
static constexpr float   ACCEL_SENS    = 0.061f / 1000.0f;  // g/LSB
static constexpr float   GYRO_SENS     = 8.75f  / 1000.0f;  // dps/LSB

// SD card (SPI HSPI bus: SCK=GPIO14, MISO=GPIO12, MOSI=GPIO13)
// GPIO15 requires a 10 kΩ pull-down to GND for ESP8266 boot compatibility
static constexpr uint8_t PIN_SD_CS     = 15;
static constexpr uint8_t SD_FLUSH_EVERY = 5;  // flush telem.csv every N rows
