# AHT20 Driver

**Files:** `../../cansat-edu-lib/aht20/aht20.hpp`, `../../cansat-edu-lib/aht20/aht20.cpp` (consumed by `base/` via `lib_extra_dirs`)

The AHT20 measures relative humidity and temperature over I2C in a single combined
measurement. The framework uses it as the humidity source; its own temperature reading
is available but not exposed through the `sensors` facade (see [config.md](config.md)
and `cansat-edu-lib/sensors`).

---

## API

### `bool AHT20::init()`

Sends the AHT20 initialization sequence (`0xBE 0x08 0x00`) and waits 10 ms. Returns
`true` if the I2C transaction succeeded.

```cpp
if (!aht20.init()) {
    // sensor not found or wiring issue
}
```

---

### `bool AHT20::read(float& temp, float& hum)`

Triggers a measurement (`0xAC 0x33 0x00`), waits 90 ms, then reads 6 status/data bytes.

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| `temp` | `float&` | °C | Temperature (AHT20's own reading) |
| `hum` | `float&` | %RH | Relative humidity |

Returns `false` if the busy bit (bit 7 of the status byte) is still set after the wait,
or if the I2C transaction failed.

```cpp
float temp, hum;
if (aht20.read(temp, hum)) {
    Serial.printf("H: %.2f%%\n", hum);
}
```

---

## Conversion

20-bit raw humidity and temperature values are packed across the 6 response bytes:

```cpp
humidity_rh  = humidityRaw    * 100.0f / 1048576.0f;
temperature_c = temperatureRaw * 200.0f / 1048576.0f - 50.0f;
```

| Bytes | Content |
|-------|---------|
| 0 | Status byte (bit 7 = busy) |
| 1–2, upper nibble of 3 | Humidity (20-bit) |
| lower nibble of 3, 4–5 | Temperature (20-bit) |

---

## Global instance

```cpp
extern AHT20 aht20;  // defined in aht20.cpp
```

Include `aht20.hpp` to use `aht20` from any translation unit.
