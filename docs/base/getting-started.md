# Getting Started

## Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/) (CLI) **or** [PlatformIO IDE](https://platformio.org/platformio-ide) (VS Code extension)
- The ESP32-S3 board's native USB (USB-Serial-JTAG) — no external USB-to-serial adapter needed
- Python 3.x (required by PlatformIO)

## Clone and build

```bash
git clone <repo-url> cansat-edu
cd cansat-edu
pio run
```

This doc covers the reference firmware in `base/`. If you're after a lesson from the
curriculum (begginer/intermediate/advanced) rather than this reference firmware, see
[`docs/download-guide.md`](../download-guide.md) instead — lessons are fetched with the
`mecseksat` CLI, not a repo clone.

PlatformIO will automatically download the ESP32 Arduino core (and the Adafruit
NeoPixel library used by the status LED) on the first build.

## Flash

```bash
pio run --target upload
```

The upload baud rate is set to `115200` in `platformio.ini`. The ESP32-S3's built-in
USB-Serial-JTAG handles reset/bootloader entry automatically — if the upload fails, try
holding BOOT while tapping RESET, then re-running the command.

## Serial monitor

```bash
pio device monitor
```

Expected output at 115200 baud:

```
BMP580 initialized.
AHT20 initialized.
TMP102 initialized.
LSM6DS3 initialized.
GNSS initialized.
Radio initialized.
SD card ready.
T: 23.45 C  P: 1013.25 hPa
H: 48.70%  (AHT20 temp: 23.48 C)
T2: 23.40 C
Accel [g]   X: 0.001  Y: -0.003  Z: 0.998
Gyro  [dps] X: 0.02  Y: -0.01  Z: 0.00
...
```

Readings print every 2 seconds (millis-gated, non-blocking). The firmware waits 200 ms
after `Serial.begin()` before printing, so the first init messages will always be
visible on a cold boot.

Every peripheral init is non-fatal — if a sensor, the GNSS module, the radio, or the SD
card is absent, the firmware logs a warning and continues with the rest. This lets you
bring the board up incrementally while assembling the panel.

## Project configuration

All hardware constants (pin numbers, I2C addresses, SPI pins, radio parameters) live in
[`include/config.hpp`](../include/config.hpp). Edit that file to adapt the firmware to a
different board revision. See [config.md](config.md) for details.

## PlatformIO environment

| Setting | Value |
|---------|-------|
| Platform | espressif32 |
| Board | esp32-s3-devkitc-1 |
| Framework | Arduino |
| Upload speed | 115200 |
| Monitor speed | 115200 |
| Extra build flags | `-Iinclude -std=gnu++17` (exposes `config.hpp` to all library modules, enables C++17) |
| Library dependency | `adafruit/Adafruit NeoPixel` (status LED) |

## See also

[overview.md](overview.md) — project architecture, hardware list, data flow.
