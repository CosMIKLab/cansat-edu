# CanSat-Edu — Project Overview

## What is this?

`cansat-edu` is an educational firmware for a CanSat (satellite-in-a-can) platform built on the ESP8266 (ESP-12E). It reads environmental and inertial data from two onboard sensors and transmits it over LoRa radio.

## Hardware

| Component | Role | Interface |
|-----------|------|-----------|
| ESP8266 ESP-12E | Microcontroller | — |
| Bosch BME280 | Temperature / Pressure / Humidity | I2C @ 0x76 |
| ST LSM6DSOX | Accelerometer + Gyroscope (IMU) | I2C @ 0x6A |
| RN2483 (or compatible) | LoRa radio transceiver | UART (Serial1) |

## Architecture

The firmware is structured as independent C++ library modules under `lib/`. Each module owns its hardware resource and exposes a minimal public API.

```
cansat-edu/
├── include/
│   └── config.hpp          ← board-wide constants (pins, addresses, sensitivity)
├── lib/
│   ├── board/              ← Serial + I2C bus initialisation
│   ├── bme280/             ← BME280 environment sensor driver
│   ├── lsm6dsox/           ← LSM6DSOX inertial sensor driver
│   └── radio/              ← LoRa radio driver (RN2483 AT command set)
├── src/
│   └── main.cpp            ← Arduino setup() / loop() entry point
└── docs/                   ← this folder
```

## Data flow

```
setup()
  └─ board.init()       → Serial @ 115200 baud (200 ms settle), I2C on SDA=4/SCL=5 @ 400 kHz
  └─ bme.init()         → verify chip ID, load calibration, leave sensor in sleep mode
  └─ imu.init()         → verify WHO_AM_I, configure ODR/range, set _ready flag
  └─ radio.init()       → open Serial1, send 5 LoRa configuration commands

loop()  (millis-gated, every 2 s)
  └─ bme.read()         → trigger forced measurement, poll status, burst-read 8 bytes → temp / press / hum
  └─ imu.read()         → guard _ready, burst-read 12 bytes, scale → ax/ay/az / gx/gy/gz
  └─ Serial.printf()    → print readings to USB serial monitor
```

## Docs index

| File | Contents |
|------|----------|
| [getting-started.md](getting-started.md) | Toolchain setup, build, flash, serial monitor |
| [hardware.md](hardware.md) | Wiring and pinout |
| [config.md](config.md) | `config.hpp` constant reference |
| [bme280.md](bme280.md) | BME280 driver API and internals |
| [lsm6dsox.md](lsm6dsox.md) | LSM6DSOX driver API and internals |
| [radio.md](radio.md) | Radio driver API and LoRa settings |
