#pragma once
#include <stdint.h>

// USB-serial console baud rate
static constexpr uint32_t SERIAL_BAUD = 115200;

// I2C0 bus
static constexpr uint8_t  PIN_SDA     = 8;
static constexpr uint8_t  PIN_SCL     = 9;
static constexpr uint32_t I2C_FREQ_HZ = 400000;

// I2C0 device addresses
static constexpr uint8_t LSM6DS3_ADDR     = 0x6A;  // IMU (accel + gyro)
static constexpr uint8_t LSM6DS3_ADDR_ALT = 0x6B;  // fallback if the primary address doesn't answer
static constexpr uint8_t BMP580_ADDR      = 0x46;  // barometric pressure + temperature
static constexpr uint8_t AHT20_ADDR       = 0x38;  // humidity + temperature
static constexpr uint8_t TMP102_ADDR      = 0x49;  // secondary temperature
static constexpr uint8_t GNSS_ADDR        = 0x42;  // u-blox SAM-M8Q, DDC/I2C streaming

// IMU sensitivity: LSM6DS3 at 416 Hz ODR, ±2 g / ±245 dps (CTRL1_XL=0x60, CTRL2_G=0x60)
static constexpr float ACCEL_SENS = 0.061f / 1000.0f;  // g/LSB
static constexpr float GYRO_SENS  = 8.75f  / 1000.0f;  // dps/LSB

// SPI2 bus — shared between the micro-SD card and the LoRa radio (separate CS lines,
// both idle-high before either device is added to the bus)
static constexpr uint8_t PIN_SPI_MOSI = 11;
static constexpr uint8_t PIN_SPI_MISO = 13;
static constexpr uint8_t PIN_SPI_SCK  = 12;

// micro-SD card (shares SPI2 above with the radio)
static constexpr uint8_t PIN_SD_CS      = 10;
static constexpr uint8_t SD_FLUSH_EVERY = 5;  // flush telem.csv every N rows

// E22-900M22S (SX126x) LoRa radio — shares SPI2 above with the SD card
static constexpr uint8_t PIN_RADIO_CS    = 2;
static constexpr uint8_t PIN_RADIO_RESET = 42;
static constexpr uint8_t PIN_RADIO_BUSY  = 41;
static constexpr uint8_t PIN_RADIO_DIO1  = 40;
static constexpr uint8_t PIN_RADIO_RXEN  = 47;

// LoRa radio parameters (SX126x, EU868 default channel)
static constexpr uint32_t RADIO_FREQ_HZ   = 868100000;
static constexpr uint8_t  RADIO_SF        = 7;   // spreading factor SF7
static constexpr uint16_t RADIO_BW_KHZ    = 125; // bandwidth
static constexpr uint8_t  RADIO_CR        = 5;   // coding rate 4/5
static constexpr int8_t   RADIO_POWER_DBM = 12;

// Status LED (WS2816B, single pixel, driven via RMT) — NOT WS2812-protocol:
// this chip is 16-bit per channel (48 bits/pixel, GRB order), not 8-bit/24-bit.
static constexpr uint8_t PIN_STATUS_LED = 1;

// CAN bus — pins reserved for a future feature, no driver implemented yet
static constexpr uint8_t PIN_CAN_TX = 4;
static constexpr uint8_t PIN_CAN_RX = 5;
