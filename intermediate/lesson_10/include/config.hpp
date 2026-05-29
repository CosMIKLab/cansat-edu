#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

static constexpr uint8_t  PIN_SDA = 4;
static constexpr uint8_t  PIN_SCL = 5;

static constexpr uint32_t BAUD_RADIO = 115200;

static constexpr uint8_t  BME280_ADDR = 0x76;
static constexpr uint8_t  LSM_ADDR    = 0x6A;

static constexpr float    ACCEL_SENS    = 0.061f / 1000.0f;
static constexpr float    GYRO_SENS     = 8.75f  / 1000.0f;

static constexpr uint8_t  PIN_SD_CS     = 15;
static constexpr uint8_t  SD_FLUSH_EVERY = 5;

#endif /* CONFIG_HPP */
