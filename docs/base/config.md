# Configuration Reference

**File:** `include/config.hpp`

This header is the single source of truth for all board-level constants. It is included by every library module (made possible by the `-Iinclude` build flag in `platformio.ini`).

---

## Serial console

```cpp
static constexpr uint32_t SERIAL_BAUD = 115200;
```

Baud rate for the USB-Serial-JTAG console, used by `Board::init()` → `Serial.begin()`.
Unlike the old ESP8266 design, the radio no longer shares this UART — it lives on its
own SPI2 bus, so the console is always available.

---

## I2C0 bus

```cpp
static constexpr uint8_t  PIN_SDA     = 8;
static constexpr uint8_t  PIN_SCL     = 9;
static constexpr uint32_t I2C_FREQ_HZ = 400000;
```

GPIO pin numbers and clock speed for the I2C0 bus shared by all five sensors.
`Board::init()` passes `PIN_SDA`/`PIN_SCL` to `Wire.begin()` and `I2C_FREQ_HZ` to
`Wire.setClock()`.

---

## I2C0 device addresses

```cpp
static constexpr uint8_t LSM6DS3_ADDR     = 0x6A;
static constexpr uint8_t LSM6DS3_ADDR_ALT = 0x6B;
static constexpr uint8_t BMP580_ADDR      = 0x46;
static constexpr uint8_t AHT20_ADDR       = 0x38;
static constexpr uint8_t TMP102_ADDR      = 0x49;
static constexpr uint8_t GNSS_ADDR        = 0x42;
```

| Constant | Device | Notes |
|----------|--------|-------|
| `LSM6DS3_ADDR` / `LSM6DS3_ADDR_ALT` | IMU | Driver tries `0x6A` first, falls back to `0x6B` |
| `BMP580_ADDR` | Barometer/temp | Fixed address, no select pin |
| `AHT20_ADDR` | Humidity/temp | Fixed address, no select pin |
| `TMP102_ADDR` | Secondary temp | `0x49` with ADD0 tied to GND |
| `GNSS_ADDR` | SAM-M8Q GNSS | Fixed u-blox DDC/I2C address |

---

## IMU sensitivity

```cpp
static constexpr float ACCEL_SENS = 0.061f / 1000.0f;  // g/LSB
static constexpr float GYRO_SENS  = 8.75f  / 1000.0f;  // dps/LSB
```

These constants convert the raw 16-bit ADC values from the LSM6DS3 into physical units. They correspond to the LSM6DS3 datasheet sensitivity values for the configured ranges:

| Constant | Range | Sensitivity (datasheet) |
|----------|-------|------------------------|
| `ACCEL_SENS` | ±2 g | 0.061 mg/LSB |
| `GYRO_SENS` | ±245 dps | 8.75 mdps/LSB |

These values are set by the `CTRL1_XL` and `CTRL2_G` register writes (both `0x60`, 416 Hz
ODR) in `LSM6DS3::init()`. If you change the range in `lsm6ds3.cpp`, update these
constants accordingly.

---

## SPI2 bus (shared: SD card + radio)

```cpp
static constexpr uint8_t PIN_SPI_MOSI = 11;
static constexpr uint8_t PIN_SPI_MISO = 13;
static constexpr uint8_t PIN_SPI_SCK  = 12;

static constexpr uint8_t PIN_SD_CS      = 10;
static constexpr uint8_t SD_FLUSH_EVERY = 5;

static constexpr uint8_t PIN_RADIO_CS    = 2;
static constexpr uint8_t PIN_RADIO_RESET = 42;
static constexpr uint8_t PIN_RADIO_BUSY  = 41;
static constexpr uint8_t PIN_RADIO_DIO1  = 40;
static constexpr uint8_t PIN_RADIO_RXEN  = 47;
```

The SD card and radio share one physical SPI bus (SPI2) with independent chip-select
lines. `Board::init()` sets both CS pins idle-high before calling `SPI.begin()`.
`SD_FLUSH_EVERY` controls how many telemetry rows accumulate before `Storage::log()`
flushes to disk.

---

## LoRa radio parameters

```cpp
static constexpr uint32_t RADIO_FREQ_HZ   = 868100000;
static constexpr uint8_t  RADIO_SF        = 7;
static constexpr uint16_t RADIO_BW_KHZ    = 125;
static constexpr uint8_t  RADIO_CR        = 5;
static constexpr int8_t   RADIO_POWER_DBM = 12;
```

| Constant | Meaning |
|----------|---------|
| `RADIO_FREQ_HZ` | EU868 default channel (868.1 MHz) |
| `RADIO_SF` | Spreading factor (SF7 = fastest / shortest range) |
| `RADIO_BW_KHZ` | LoRa bandwidth |
| `RADIO_CR` | Coding rate denominator (5 = 4/5) |
| `RADIO_POWER_DBM` | Transmit power |

See [radio.md](radio.md) for how these feed into the SX126x command set.

---

## Status LED

```cpp
static constexpr uint8_t PIN_STATUS_LED = 1;
```

GPIO driving the single WS2816B (16-bit/channel, NOT WS2812-protocol) pixel. See [led.md](led.md).

---

## CAN bus (reserved)

```cpp
static constexpr uint8_t PIN_CAN_TX = 4;
static constexpr uint8_t PIN_CAN_RX = 5;
```

Pins reserved for a future CAN/TWAI feature — no driver implemented yet.
