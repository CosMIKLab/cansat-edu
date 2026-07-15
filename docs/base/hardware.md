# Hardware & Wiring

## Microcontroller — ESP32-S3

- 240 MHz dual-core Xtensa LX7
- 320 KB SRAM, 8 MB flash
- 3.3 V logic — **do not connect 5 V signals directly**
- Native USB-Serial-JTAG — no external USB-to-serial adapter needed

## I2C0 bus

All five I2C sensors share the same bus running at **400 kHz** (fast mode). Pin
assignments and clock speed are set in `Board::init()` via `Wire.begin()` +
`Wire.setClock()`, using constants from `include/config.hpp`:

| Signal | ESP32-S3 GPIO | Default pin number |
|--------|---------------|--------------------|
| SDA | GPIO8 | 8 |
| SCL | GPIO9 | 9 |

```
ESP32-S3              BMP580 / AHT20 / TMP102 / LSM6DS3 / SAM-M8Q
  GPIO8 (SDA) ───────── SDA
  GPIO9 (SCL) ───────── SCL
  3.3V        ───────── VCC
  GND         ───────── GND
```

Pull-up resistors (4.7 kΩ) are required on SDA and SCL to 3.3 V unless the breakout
boards include them.

## BMP580

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |

Fixed I2C address `0x46` — no address-select pin on this part.

## AHT20

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |

Fixed I2C address `0x38`.

## TMP102

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |
| ADD0 | GND (sets I2C address to 0x49) |

## LSM6DS3

| Pin | Connection |
|-----|-----------|
| VDD | 3.3 V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |
| SA0 | GND or 3.3 V (sets I2C address to 0x6A or 0x6B) |
| CS  | 3.3 V (selects I2C mode) |

> The driver probes `0x6A` first and automatically falls back to `0x6B` if it doesn't answer.

## SAM-M8Q (GNSS)

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |

Fixed I2C (DDC) address `0x42`. The module streams NMEA sentences continuously; the
driver polls a pending-byte-count register and reads whatever is available each cycle.

## SPI2 bus — shared between the micro-SD card and the LoRa radio

| Signal | ESP32-S3 GPIO |
|--------|---------------|
| SCK | GPIO12 |
| MOSI | GPIO11 |
| MISO | GPIO13 |
| SD CS | GPIO10 |
| Radio CS | GPIO2 |

Both chip-selects are configured idle-high before either device is added to the bus, so
they can share the same clock/data lines without interfering with each other.

## E22-900M22S (SX1262 core) LoRa radio

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SCK / MOSI / MISO | shared SPI2 (see above) |
| CS | GPIO2 |
| RESET | GPIO42 |
| BUSY | GPIO41 |
| DIO1 | GPIO40 |
| RXEN | GPIO47 (antenna-switch control, driven by the driver — no DIO2 RF-switch wiring) |

## WS2816B status LED

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V or 5 V (per LED datasheet) |
| GND | GND |
| DIN | GPIO1 |

Single-pixel, single-wire NZR protocol like WS2812 — but **16-bit per channel**
(48 bits/pixel, GRB order), not the 8-bit/24-bit WS2812 protocol. Driven directly
via RMT (`esp32-hal-rmt.h`); see [led.md](led.md) for details and the bug this
distinction caused during hardware bring-up.

## CAN bus (reserved, not implemented)

| Pin | Connection |
|-----|-----------|
| TX | GPIO4 |
| RX | GPIO5 |

Pins are reserved on the board and in `config.hpp` for a future CAN/TWAI feature — no
driver exists yet.

## Full wiring diagram (ASCII)

```
                   3.3V ── VCC (BMP580, AHT20, TMP102, LSM6DS3, SAM-M8Q, radio, LED)
                   GND  ── GND (all)

ESP32-S3 GPIO8 ──[4.7k]── SDA ── BMP580 / AHT20 / TMP102 / LSM6DS3 / SAM-M8Q SDA
ESP32-S3 GPIO9 ──[4.7k]── SCL ── BMP580 / AHT20 / TMP102 / LSM6DS3 / SAM-M8Q SCL

ESP32-S3 GPIO12 (SCK)  ──┬── SD card SCK
ESP32-S3 GPIO11 (MOSI) ──┼── SD card MOSI      ──┬── Radio MOSI
ESP32-S3 GPIO13 (MISO) ──┴── SD card MISO      ──┴── Radio MISO
ESP32-S3 GPIO10         ──── SD card CS
ESP32-S3 GPIO2          ──── Radio CS
ESP32-S3 GPIO42/41/40/47 ─── Radio RESET/BUSY/DIO1/RXEN

ESP32-S3 GPIO1 ──────────── WS2816B LED DIN
```
