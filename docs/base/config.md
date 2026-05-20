# Configuration Reference

**File:** `include/config.hpp`

This header is the single source of truth for all board-level constants. It is included by every library module (made possible by the `-Iinclude` build flag in `platformio.ini`).

---

## I2C bus

```cpp
static constexpr uint8_t PIN_SDA = 4;
static constexpr uint8_t PIN_SCL = 5;
```

GPIO pin numbers for the I2C bus. Change these if your board uses different pins. The `Board::init()` function passes them to `Wire.begin()`.

---

## UART baud rates

```cpp
static constexpr uint32_t BAUD_RADIO = 115200;
```

| Constant | Used by | Purpose |
|----------|---------|---------|
| `BAUD_RADIO` | `Board::init()` → `Serial.begin()` and `Radio::init()` | Shared baud rate for USB monitor and LoRa module (same physical UART, routed by switch) |

---

## I2C addresses

```cpp
static constexpr uint8_t BME280_ADDR = 0x76;
static constexpr uint8_t LSM_ADDR    = 0x6A;
```

| Constant | Default | Alternate | How to select alternate |
|----------|---------|-----------|------------------------|
| `BME280_ADDR` | `0x76` | `0x77` | Pull SDO HIGH |
| `LSM_ADDR` | `0x6A` | `0x6B` | Pull SA0 HIGH |

---

## IMU sensitivity

```cpp
static constexpr float ACCEL_SENS = 0.061f / 1000.0f;  // g/LSB
static constexpr float GYRO_SENS  = 8.75f  / 1000.0f;  // dps/LSB
```

These constants convert the raw 16-bit ADC values from the LSM6DSOX into physical units. They correspond to the LSM6DSOX datasheet sensitivity values for the configured ranges:

| Constant | Range | Sensitivity (datasheet) |
|----------|-------|------------------------|
| `ACCEL_SENS` | ±2 g | 0.061 mg/LSB |
| `GYRO_SENS` | ±250 dps | 8.75 mdps/LSB |

These values are set by the `CTRL1_XL` and `CTRL2_G` register writes in `LSM6DSOX::init()`. If you change the range in `lsm6dsox.cpp`, update these constants accordingly.

| Accel range | CTRL1_XL[3:2] | Sensitivity |
|-------------|---------------|-------------|
| ±2 g | `00` | 0.061 mg/LSB |
| ±4 g | `10` | 0.122 mg/LSB |
| ±8 g | `11` | 0.244 mg/LSB |
| ±16 g | `01` | 0.488 mg/LSB |

| Gyro range | CTRL2_G[3:1] | Sensitivity |
|------------|--------------|-------------|
| ±125 dps | `001` | 4.375 mdps/LSB |
| ±250 dps | `000` | 8.75 mdps/LSB |
| ±500 dps | `010` | 17.50 mdps/LSB |
| ±1000 dps | `100` | 35.00 mdps/LSB |
| ±2000 dps | `110` | 70.00 mdps/LSB |
