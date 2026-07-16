# Status LED Driver

**Files:** `../../cansat-edu-lib/led/led.hpp`, `../../cansat-edu-lib/led/led.cpp` (consumed by `base/` via `lib_extra_dirs`)

A single WS2816B addressable LED used to show boot and status state at a glance,
without needing a serial monitor attached. Driven directly via the RMT peripheral
through the ESP32 Arduino core's `esp32-hal-rmt.h` wrapper (no external library).

**WS2816B is not WS2812-protocol-compatible**, despite the similar naming and
single-wire NZR signaling — it's a **16-bit-per-channel** chip (48 bits per pixel:
16-bit G, 16-bit R, 16-bit B, MSB-first, GRB order), not the 8-bit-per-channel
(24-bit) protocol WS2812/WS2812B use. An earlier version of this driver assumed
WS2812 compatibility and only ever sent 24 bits per pixel; on real hardware this
resulted in every color command showing up as some brightness of green only, since
the chip's shift register only ever received enough bits to fill its first (green)
channel before the frame ended. Confirmed and fixed against real hardware — see the
WS2816B datasheet if extending this driver further.

---

## API

### `void StatusLed::init()`

Initialises the RMT channel (1 pixel, GPIO from `PIN_STATUS_LED`, 50 ns/tick) and
clears the LED.

```cpp
led.init();
```

---

### `void StatusLed::set(uint8_t r, uint8_t g, uint8_t b)`

Sets the pixel to the given RGB color and pushes it out immediately. Takes 8-bit
values for API convenience; internally scaled to the chip's native 16-bit-per-channel
range by byte-duplication (`0xFF` → `0xFFFF`, `0x00` → `0x0000`).

```cpp
led.set(0, 32, 0);  // dim green
```

---

### `void StatusLed::clear()`

Turns the LED off.

```cpp
led.clear();
```

---

## Notes

- The driver builds the raw 48-bit NZR pulse train itself (GRB channel order, MSB
  first per channel) and hands it to `rmtWriteBlocking()` — there's no vendor
  library involved.
- Bit timing: `T0H`=400ns/`T0L`=850ns, `T1H`=800ns/`T1L`=450ns — the same NZR timing
  family as WS2812, just applied to 48 bits per pixel instead of 24.
- Keep brightness low (values well under 255) during development — this LED at full
  brightness is very bright at close range.
- `base/src/main.cpp` uses the LED for a simple booting → ready pattern; lesson content
  can build richer status/fault patterns on top of `set()`/`clear()`.

---

## Global instance

```cpp
extern StatusLed led;  // defined in led.cpp
```

Include `led.hpp` to use `led` from any translation unit.
