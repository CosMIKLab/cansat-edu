# Radio Driver

**Files:** `lib/radio/radio.hpp`, `lib/radio/radio.cpp`

The radio module drives a LoRa transceiver (RN2483 or compatible) using its ASCII AT-command UART interface. The ESP8266 `Serial1` peripheral is used (TX on GPIO2).

---

## API

### `bool Radio::init()`

Opens `Serial1` at `BAUD_RADIO` and sends the 5 configuration commands to the radio module. Each command is written as:

```
radio set <setting>\r\n
```

Returns `true` unconditionally (the current implementation does not parse the module's response).

```cpp
if (!radio.init()) {
    Serial.println("Radio init failed!");
}
```

---

### `bool Radio::send(const uint8_t* payload, uint8_t len)`

Transmits a byte buffer as a hex-encoded LoRa packet using the RN2483 `radio tx` command:

```
radio tx <hex>\r\n
```

Each byte is zero-padded to two hex digits. Example: `{0x01, 0xAB, 0x0F}` → `radio tx 01AB0F\r\n`.

| Parameter | Description |
|-----------|-------------|
| `payload` | Pointer to byte array to transmit |
| `len` | Number of bytes in the array |

Returns `true` unconditionally.

```cpp
uint8_t data[] = {0x01, 0x02, 0x03};
radio.send(data, sizeof(data));
```

---

## LoRa configuration

The 5 settings sent during `init()` are stored in the private `_settings[]` array:

| Command | Value | Meaning |
|---------|-------|---------|
| `radio set mod lora` | — | Set modulation to LoRa |
| `radio set freq 868100000` | 868.1 MHz | EU ISM band channel 0 |
| `radio set sf sf7` | SF7 | Spreading factor 7 (shortest range, fastest) |
| `radio set pa off` | — | Disable power amplifier path |
| `radio set pwr 12` | 12 dBm | Transmit power |

To change a setting, edit the `_settings` array in `radio.cpp`.

---

## Changing LoRa settings

Common spreading factor / range tradeoffs:

| SF | Airtime (typical) | Range |
|----|-------------------|-------|
| SF7 | ~50 ms | shortest |
| SF9 | ~200 ms | medium |
| SF12 | ~2000 ms | longest |

CanSat flights typically use SF7 or SF9 to keep latency low during the descent phase.

---

## Notes

- **TX only:** The ESP-12E exposes `Serial1` as TX-only (GPIO2). Receiving acknowledgments from the RN2483 requires wiring GPIO3 (U0RXD) and swapping to `Serial` with software remapping, or using a GPIO with SoftwareSerial.
- **Duty cycle:** The 868 MHz EU band enforces a 1% duty cycle. At SF7 with ~50 ms packets, the maximum safe transmission rate is roughly one packet per 5 seconds.
- **Initialisation order:** `Radio::init()` must be called after `Board::init()` because it depends on `Serial1`, which is part of the Arduino framework initialised during `board.init()`.

---

## Global instance

```cpp
extern Radio radio;  // defined in radio.cpp
```

Include `radio.hpp` to use `radio` from any translation unit.
