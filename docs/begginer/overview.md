# Beginner Framework — Overview

**Location:** `begginer/`

The beginner framework wraps the full CanSat hardware stack behind a minimal API designed for students aged 12–16. A student edits exactly one file (`src/mission.cpp`) and runs one command (`./cansat run`). PlatformIO, sensor drivers, and register-level code are hidden entirely.

---

## Architecture

```
begginer/
├── cansat                 ← CLI (Mac/Linux)
├── cansat.bat             ← CLI (Windows)
├── OLVASSEL.md            ← Hungarian student guide
├── mission_template.cpp   ← pristine starter (cansat new copies this)
├── platformio.ini
├── include/
│   ├── config.hpp         ← hardware constants (internal)
│   └── cansat.h           ← single student include
├── lib/
│   ├── board/             ← Serial + I2C init (internal)
│   ├── bme280/            ← BME280 driver (internal)
│   ├── lsm6dsox/          ← LSM6DSOX driver (internal)
│   ├── storage/           ← SD card driver (internal)
│   ├── sensors/           ← Sensors wrapper (student-facing)
│   ├── radio_easy/        ← RadioEasy wrapper (student-facing)
│   ├── wifi_easy/         ← WiFiEasy wrapper (student-facing)
│   └── sd_easy/           ← SdEasy wrapper (student-facing)
└── src/
    ├── main.cpp           ← Arduino glue (internal — do not edit)
    └── mission.cpp        ← STUDENT FILE
```

`begginer/` is **fully self-contained** — it does not depend on `base/` or any other directory.

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
| [getting-started.md](getting-started.md) | Toolchain, first run, CLI reference |
| [api.md](api.md) | Full API reference for all framework objects |
