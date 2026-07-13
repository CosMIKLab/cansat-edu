# Beginner Framework — Overview

**Location:** `cansat-edu-lib/` (this repo)

The beginner framework wraps the full CanSat hardware stack behind a minimal API designed
for students aged 12–16. A student edits exactly one file (`src/mission.cpp`) and runs one
command (`mecseksat run`). PlatformIO, sensor drivers, and register-level code are hidden
entirely.

This repo ships the framework library itself, openly. The begginer *lesson content* (10
chapters — README, HINTS, starter code) lives in a separate, access-controlled repo and is
fetched per-lesson with the `mecseksat` CLI — see [`../download-guide.md`](../download-guide.md).
A downloaded lesson's `platformio.ini` references this library via
`lib_extra_dirs = ../../cansat-edu-lib`; `mecseksat get begginer/N` fetches both the lesson
and this library automatically, so a student never has to think about the split.

---

## Architecture

```
cansat-edu-lib/
├── library.json / library.properties
├── board/          ← Serial + I2C init (internal)
├── bme280/         ← BME280 driver (internal)
├── lsm6dsox/       ← LSM6DSOX driver (internal)
├── storage/        ← SD card driver (internal)
├── sensors/        ← Sensors wrapper (student-facing)
├── radio_easy/     ← RadioEasy wrapper (student-facing)
├── wifi_easy/      ← WiFiEasy wrapper (student-facing)
├── sd_easy/        ← SdEasy wrapper (student-facing)
└── check/
    └── check_hardware.cpp   ← swapped in for `mecseksat check`
```

A downloaded lesson's own layout (in the content repo) is:

```
begginer/lesson_N/
├── .cansat-lesson.json    ← written by `mecseksat get`, read by `mecseksat update`
├── OLVASSEL.md            ← Hungarian student guide
├── mission_template.cpp   ← pristine starter (`mecseksat new` copies this)
├── platformio.ini         ← lib_extra_dirs = ../../cansat-edu-lib
├── include/
│   ├── config.hpp         ← hardware constants (internal)
│   └── cansat.h           ← single student include
└── src/
    ├── main.cpp           ← Arduino glue (internal — do not edit)
    └── mission.cpp        ← STUDENT FILE
```

---

## Student code shape

```cpp
#include <cansat.h>

void mission_setup() { /* runs once at boot */ }
void mission_loop()  { /* runs every 2 seconds */ }
```

`main.cpp` (hidden) calls `board.init()`, `sensors.begin()`, `radio.begin()`, `sd.begin()`, then delegates to the student's two functions.

---

## Hardware

Same PCB as the other levels:

| Component | Role | Interface |
|-----------|------|-----------|
| ESP8266 ESP-12E | Microcontroller | — |
| BME280 | Temp / Pressure / Humidity | I2C @ 0x76 |
| LSM6DSOX | Accelerometer + Gyroscope | I2C @ 0x6A |
| RN2483 | LoRa transceiver | UART (shared Serial, via switch) |
| SD card | Data logging | SPI (GPIO15 CS) |

---

## Docs index

| File | Contents |
|------|----------|
| [getting-started.md](getting-started.md) | Installing `mecseksat`, first lesson download, CLI reference |
| [api.md](api.md) | Full API reference for all framework objects |
| [`../download-guide.md`](../download-guide.md) | The general (all-tracks) download tutorial |
