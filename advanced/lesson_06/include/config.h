#pragma once
#include "driver/gpio.h"
#include "driver/uart.h"

// I2C bus
#define PIN_SDA         GPIO_NUM_8
#define PIN_SCL         GPIO_NUM_9

// I2C sensor addresses
#define BME280_ADDR     0x76
#define LSM_ADDR        0x6A

// IMU sensitivity  (±2 g / ±250 dps at 104 Hz)
#define ACCEL_SENS      (0.061f / 1000.0f)  // g/LSB
#define GYRO_SENS       (8.75f  / 1000.0f)  // dps/LSB

// SD card (SPI)
#define PIN_SD_CS       GPIO_NUM_10
#define PIN_SD_CLK      GPIO_NUM_12
#define PIN_SD_MOSI     GPIO_NUM_11
#define PIN_SD_MISO     GPIO_NUM_13
#define SD_FLUSH_EVERY  5

// LoRa radio (UART1)
#define UART_RADIO      UART_NUM_1
#define PIN_RADIO_TX    GPIO_NUM_17
#define PIN_RADIO_RX    GPIO_NUM_18

// GPIO example pins (lesson 4)
#define PIN_LED         GPIO_NUM_2
#define PIN_BUTTON      GPIO_NUM_0
