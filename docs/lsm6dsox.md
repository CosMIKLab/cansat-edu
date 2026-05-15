# LSM6DSOX Driver

**Files:** `lib/lsm6dsox/lsm6dsox.hpp`, `lib/lsm6dsox/lsm6dsox.cpp`

The LSM6DSOX is an ST 6-axis IMU combining a 3-axis accelerometer and a 3-axis gyroscope. This driver communicates via I2C and reads both sensors in a single 12-byte burst.

---

## Data type

```cpp
struct ImuData {
    float ax, ay, az;  // Accelerometer in g
    float gx, gy, gz;  // Gyroscope in dps
};
```

| Field | Unit | Description |
|-------|------|-------------|
| `ax`, `ay`, `az` | g (9.81 m/s²) | Linear acceleration along X, Y, Z |
| `gx`, `gy`, `gz` | dps (°/s) | Angular rate around X, Y, Z |

---

## API

### `bool LSM6DSOX::init()`

Initialises the IMU. Must be called once in `setup()`.

1. Reads the WHO_AM_I register (`0x0F`) and verifies it equals `0x6C`. Returns `false` if the device is not found.
2. Configures the accelerometer: 104 Hz ODR, ±2 g range (`CTRL1_XL = 0x40`).
3. Configures the gyroscope: 104 Hz ODR, ±250 dps range (`CTRL2_G = 0x40`).

Returns `true` on success.

```cpp
if (!imu.init()) {
    // device not found or wiring issue
}
```

---

### `bool LSM6DSOX::read(ImuData& data)`

Reads a single sample from both sensors in one I2C transaction. Burst-reads 12 bytes starting at `OUTX_L_G` (`0x22`), which is the first gyroscope output register. The accelerometer output registers follow immediately.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `ImuData&` | Output struct, populated on success |

Returns `true` if exactly 12 bytes were received; `false` otherwise.

```cpp
ImuData d;
if (imu.read(d)) {
    Serial.printf("A: %.3f %.3f %.3f  G: %.2f %.2f %.2f\n",
        d.ax, d.ay, d.az, d.gx, d.gy, d.gz);
}
```

---

## Register map

| Register | Address | Description |
|----------|---------|-------------|
| `REG_WHO_AM_I` | `0x0F` | Device ID — reads `0x6C` |
| `REG_CTRL1_XL` | `0x10` | Accelerometer control (ODR, range) |
| `REG_CTRL2_G` | `0x11` | Gyroscope control (ODR, range) |
| `REG_OUTX_L_G` | `0x22` | Gyro X low byte — start of 12-byte output block |

---

## Burst read layout

The 12-byte block starting at `0x22`:

| Bytes | Content |
|-------|---------|
| 0–1 | Gyro X (LSB first) |
| 2–3 | Gyro Y (LSB first) |
| 4–5 | Gyro Z (LSB first) |
| 6–7 | Accel X (LSB first) |
| 8–9 | Accel Y (LSB first) |
| 10–11 | Accel Z (LSB first) |

All values are signed 16-bit two's complement integers in little-endian order.

---

## Scaling

Raw values are converted to physical units using the sensitivity constants from `config.hpp`:

```
physical = raw_int16 × SENSITIVITY
```

| Axis | Sensitivity constant | Value |
|------|---------------------|-------|
| Accel | `ACCEL_SENS` | 0.061 × 10⁻³ g/LSB |
| Gyro | `GYRO_SENS` | 8.75 × 10⁻³ dps/LSB |

These match the datasheet values for ±2 g / ±250 dps. See [config.md](config.md) for the full range/sensitivity table.

---

## Global instance

```cpp
extern LSM6DSOX imu;  // defined in lsm6dsox.cpp
```

Include `lsm6dsox.hpp` to use `imu` from any translation unit.
