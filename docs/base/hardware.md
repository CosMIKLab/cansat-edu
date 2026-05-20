# Hardware & Wiring

## Microcontroller — ESP8266 ESP-12E

- 80 MHz Xtensa LX106 core
- 80 KB SRAM, 4 MB flash
- 3.3 V logic — **do not connect 5 V signals directly**
- I2C is bit-banged via the Arduino Wire library (any two GPIO pins can be used)

## I2C bus

Both sensors share the same I2C bus running at **400 kHz** (fast mode). Pin assignments and clock speed are set in `Board::init()` via `Wire.begin()` + `Wire.setClock(400000)`, using constants from `include/config.hpp`:

| Signal | ESP-12E GPIO | Default pin number |
|--------|--------------|--------------------|
| SDA | GPIO4 | 4 |
| SCL | GPIO5 | 5 |

```
ESP-12E               BME280 / LSM6DSOX
  GPIO4 (SDA) ───────── SDA
  GPIO5 (SCL) ───────── SCL
  3.3V        ───────── VCC
  GND         ───────── GND
```

Pull-up resistors (4.7 kΩ) are required on SDA and SCL to 3.3 V unless the breakout boards include them.

## BME280

| Pin | Connection |
|-----|-----------|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO4 |
| SCL | GPIO5 |
| SDO | GND (sets I2C address to 0x76) |
| CSB | 3.3 V (selects I2C mode) |

> Pulling SDO HIGH changes the I2C address to 0x77. Update `BME280_ADDR` in `config.hpp` if needed.

## LSM6DSOX

| Pin | Connection |
|-----|-----------|
| VDD | 3.3 V |
| GND | GND |
| SDA | GPIO4 |
| SCL | GPIO5 |
| SA0 | GND (sets I2C address to 0x6A) |
| CS  | 3.3 V (selects I2C mode) |

> Pulling SA0 HIGH changes the I2C address to 0x6B. Update `LSM_ADDR` in `config.hpp` if needed.

## Radio — RN2483 (LoRa)

The radio module communicates over UART using AT-style commands. The ESP8266 `Serial` (UART0, TX on GPIO1) is **shared** between the USB-to-serial adapter and the RN2483 via a physical switch on the board.

| Signal | ESP-12E | Description |
|--------|---------|-------------|
| TX | GPIO1 (Serial TX) | Routed by switch to USB adapter or RN2483 RX |
| Switch | — | **USB** position for monitoring/flashing; **Radio** position for flight |
| VCC | 3.3 V | — |
| GND | GND | — |

> Set the switch to **Radio** before a flight so AT commands reach the RN2483. Set it back to **USB** for serial monitoring or flashing.

## Full wiring diagram (ASCII)

```
                   3.3V ── VCC (BME280)
                        ── VCC (LSM6DSOX)
                        ── CSB (BME280)
                        ── CS  (LSM6DSOX)
                   GND  ── GND (all)
                        ── SDO (BME280)   → I2C addr 0x76
                        ── SA0 (LSM6DSOX) → I2C addr 0x6A

ESP-12E GPIO4 ──[4.7k]── SDA ── BME280 SDA
                              └─ LSM6DSOX SDA
ESP-12E GPIO5 ──[4.7k]── SCL ── BME280 SCL
                              └─ LSM6DSOX SCL

ESP-12E GPIO1 (Serial TX) ──[SWITCH]──┬── RN2483 RX       (switch → Radio)
                                      └── USB-serial RX   (switch → USB)
```
