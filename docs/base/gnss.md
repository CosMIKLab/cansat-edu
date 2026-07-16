# GNSS Driver

**Files:** `../../cansat-edu-lib/gnss/gnss.hpp`, `../../cansat-edu-lib/gnss/gnss.cpp` (consumed by `base/` via `lib_extra_dirs`)

The SAM-M8Q is a u-blox GNSS receiver that streams NMEA sentences over its I2C (DDC)
interface. This driver polls a pending-byte-count register, reads whatever data is
available, and extracts the most recent complete NMEA sentence. It does **not** parse
coordinates — it hands you the raw sentence text, matching the scope of the reference
firmware this was ported from.

---

## Data type

```cpp
struct GnssData {
    bool     present;
    uint16_t pendingBytes;
    char     nmea[96];
};
```

| Field | Description |
|-------|-------------|
| `present` | Whether the module acknowledged on the I2C bus during `init()` |
| `pendingBytes` | Byte count reported by the module's data-stream register this cycle |
| `nmea` | Most recent complete `$...` sentence extracted from the stream, or empty |

---

## API

### `bool GNSS::init()`

Probes the module's I2C address. Non-fatal if absent — the GNSS receiver is optional
hardware.

```cpp
if (!gnss.init()) {
    // module not present — this is not necessarily an error
}
```

---

### `bool GNSS::read(GnssData& data)`

1. Reads the 2-byte pending-byte-count register (`0xFD`).
2. If bytes are pending, reads up to 32 bytes from the data-stream register (`0xFF`).
3. Appends the new bytes to a 192-byte rolling ASCII window.
4. Scans the window for the last `$...\n` sequence and copies it into `data.nmea`.

```cpp
GnssData d;
if (gnss.read(d) && d.nmea[0] != '\0') {
    Serial.printf("GNSS: %s\n", d.nmea);
}
```

---

## Register map

| Register | Address | Description |
|----------|---------|-------------|
| `REG_BYTES_HIGH` | `0xFD` | 2-byte big-endian pending byte count |
| `REG_DATA_STREAM` | `0xFF` | Streaming data register — each read returns the next available bytes |

---

## Notes

- Chunk size per `read()` call is capped at 32 bytes to stay well within the Arduino
  Wire library's default buffer size.
- A `pendingBytes` value of `0` or `0xFFFF` means no new data this cycle — `read()`
  still returns `true` but leaves `nmea` empty.
- Parsing latitude/longitude out of the NMEA text (e.g. `$GNRMC`/`$GNGGA` sentences) is
  left as an exercise for lesson content — this driver only gives you the raw sentence.

---

## Global instance

```cpp
extern GNSS gnss;  // defined in gnss.cpp
```

Include `gnss.hpp` to use `gnss` from any translation unit.
