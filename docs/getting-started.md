# Getting Started

## Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/) (CLI) **or** [PlatformIO IDE](https://platformio.org/platformio-ide) (VS Code extension)
- USB-to-serial adapter or board with built-in USB (the ESP-12E bare module needs an external adapter)
- Python 3.x (required by PlatformIO)

## Clone and build

```bash
git clone <repo-url> cansat-edu
cd cansat-edu
pio run
```

PlatformIO will automatically download the ESP8266 Arduino core on the first build.

## Flash

Connect the board in flash mode (GPIO0 pulled LOW, then reset), then:

```bash
pio run --target upload
```

The upload baud rate is set to `115200` in `platformio.ini`. If the upload fails, try resetting the board and re-running the command.

## Serial monitor

```bash
pio device monitor
```

Expected output at 115200 baud:

```
BME280 initialized.
LSM6DSOX initialized.
Radio initialized.
T: 23.45 C  P: 1013.25 hPa  H: 48.70%
Accel [g]   X: 0.001  Y: -0.003  Z: 0.998
Gyro  [dps] X: 0.02  Y: -0.01  Z: 0.00
...
```

Readings print every 2 seconds.

## Project configuration

All hardware constants (pin numbers, I2C addresses, baud rates) live in [`include/config.hpp`](../include/config.hpp). Edit that file to adapt the firmware to a different board variant. See [config.md](config.md) for details.

## PlatformIO environment

| Setting | Value |
|---------|-------|
| Platform | espressif8266 |
| Board | esp12e |
| Framework | Arduino |
| Upload speed | 115200 |
| Monitor speed | 115200 |
| Extra build flag | `-Iinclude` (exposes `config.hpp` to all library modules) |
