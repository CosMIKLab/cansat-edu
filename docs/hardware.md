# Hardware & Wiring

## Microcontroller — ESP8266 ESP-12E

- 80 MHz Xtensa LX106 core
- 80 KB SRAM, 4 MB flash
- 3.3 V logic — **do not connect 5 V signals directly**
- I2C is bit-banged via the Arduino Wire library (any two GPIO pins can be used)

## I2C bus

Both sensors share the same I2C bus. Pin assignments are defined in `include/config.hpp`:

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

The radio module communicates over UART using AT-style commands. The ESP8266 `Serial1` peripheral is used (TX only on GPIO2; RX is not available on ESP-12E without remapping).

| Signal | ESP-12E | RN2483 |
|--------|---------|--------|
| TX | GPIO2 (Serial1 TX) | RX |
| RX | — (not connected) | TX |
| VCC | 3.3 V | 3.3 V |
| GND | GND | GND |

> The current firmware only transmits; incoming responses from the RN2483 are not parsed. Full bidirectional communication requires wiring RX and adding response handling in `radio.cpp`.

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

ESP-12E GPIO2 (Serial1 TX) ─────────────── RN2483 RX
```
