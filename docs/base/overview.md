# CanSat-Edu — Project Overview

## What is this?

`cansat-edu` is an educational firmware for a CanSat (satellite-in-a-can) platform built
on the ESP32-S3 ("Mecsek Explorer" panel). It reads environmental, inertial, and
positional data from five onboard sensors, drives a status LED, and transmits telemetry
over a LoRa radio.

## Hardware

| Component | Role | Interface |
|-----------|------|-----------|
| ESP32-S3 | Microcontroller | — |
| Bosch BMP580 | Barometric pressure / temperature | I2C0 @ 0x46 |
| AHT20 | Humidity / temperature | I2C0 @ 0x38 |
| TMP102 | Secondary temperature | I2C0 @ 0x49 |
| ST LSM6DS3 | Accelerometer + Gyroscope (IMU) | I2C0 @ 0x6A (fallback 0x6B) |
| u-blox SAM-M8Q | GNSS receiver | I2C0 @ 0x42 (DDC) |
| E22-900M22S (SX1262) | LoRa radio transceiver | SPI2 (shared with SD card) |
| WS2816B | Addressable status LED | GPIO1 (RMT) |

## Architecture

The firmware itself (`base/`) has no in-repo `lib/` — `base/lib/` is empty. Driver
modules are independent C++ library modules that live in the sibling
`cansat-edu-lib/` directory and are pulled in via `lib_extra_dirs =
../cansat-edu-lib` in `base/platformio.ini`. Each module owns its hardware
resource and exposes a minimal public API.

```
cansat-edu/
├── base/
│   ├── include/
│   │   └── config.hpp      ← board-wide constants (pins, addresses, sensitivity, radio params)
│   ├── src/
│   │   └── main.cpp        ← Arduino setup() / loop() entry point
│   └── platformio.ini      ← lib_extra_dirs = ../cansat-edu-lib
├── cansat-edu-lib/
│   ├── board/              ← Serial + I2C + shared SPI2 bus initialisation
│   ├── bmp580/              ← BMP580 pressure/temperature driver
│   ├── aht20/               ← AHT20 humidity/temperature driver
│   ├── tmp102/               ← TMP102 secondary temperature driver
│   ├── lsm6ds3/             ← LSM6DS3 inertial sensor driver
│   ├── gnss/                ← SAM-M8Q GNSS/NMEA driver
│   ├── led/                 ← WS2816B status LED driver
│   ├── storage/              ← SD card CSV/event logger (shares SPI2 with radio)
│   ├── wifi_easy/            ← WiFi + HTTP client wrapper
│   └── radio/                ← LoRa radio driver (E22-900M22S / SX126x command set)
└── docs/                   ← this folder
```

## Data flow

```
setup()
  └─ board.init()       → Serial @ 115200 baud, I2C0 on SDA=8/SCL=9 @ 400 kHz,
                           SPI2 on SCK=12/MISO=13/MOSI=11 (SD + radio CS idle-high)
  └─ led.init()         → status LED ready
  └─ bmp580.init()      → verify chip ID, configure OSR/ODR, leave sensor in standby
  └─ aht20.init()       → send init sequence
  └─ tmp102.init()      → probe I2C address
  └─ imu.init()         → verify WHO_AM_I (with 0x6A/0x6B fallback), configure ODR/range
  └─ gnss.init()        → probe I2C address (optional — non-fatal if absent)
  └─ radio.init()       → reset SX126x, verify SPI link, configure LoRa modem
  └─ Storage::init()    → mount SD card, open telem.csv + events.txt

loop()  (millis-gated, every 2 s)
  └─ bmp580.read()      → forced-mode measurement → temp / press
  └─ aht20.read()       → measurement → humidity (+ its own temp, discarded)
  └─ tmp102.read()      → secondary temperature
  └─ imu.read()         → burst-read 12 bytes, scale → ax/ay/az / gx/gy/gz
  └─ gnss.read()        → poll pending-byte count, stream NMEA, extract latest sentence
  └─ Serial.printf()    → print readings to USB serial monitor
  └─ Storage::log()     → append one CSV row with all of the above
```

## Docs index

| File | Contents |
|------|----------|
| [getting-started.md](getting-started.md) | Toolchain setup, build, flash, serial monitor |
| [hardware.md](hardware.md) | Wiring and pinout |
| [config.md](config.md) | `config.hpp` constant reference |
| [bmp580.md](bmp580.md) | BMP580 driver API and internals |
| [aht20.md](aht20.md) | AHT20 driver API and internals |
| [tmp102.md](tmp102.md) | TMP102 driver API and internals |
| [lsm6ds3.md](lsm6ds3.md) | LSM6DS3 driver API and internals |
| [gnss.md](gnss.md) | GNSS driver API and NMEA handling |
| [led.md](led.md) | Status LED driver API |
| [radio.md](radio.md) | Radio driver API and LoRa/SX126x settings |
