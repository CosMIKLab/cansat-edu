# Radio Driver

**Files:** `../../cansat-edu-lib/radio/radio.hpp`, `../../cansat-edu-lib/radio/radio.cpp` (+ shared `../../cansat-edu-lib/radio_proto`; consumed by `base/` via `lib_extra_dirs`)

The radio module drives an E22-900M22S LoRa transceiver (SX1262 core) over its own SPI2
bus (shared with the SD card, separate chip-select). Unlike the previous UART/AT-command
design, this is a register-level SX126x command driver — no physical USB/Radio switch
exists anymore, since the radio no longer shares the console UART.

This is the newest and least field-tested part of the firmware: it implements the
standard SX126x command sequence (documented publicly by Semtech and widely used by
libraries such as RadioLib) from scratch, since the hardware's own reference firmware
only implements a `GetStatus` presence check. Treat the TX/RX timing and IRQ handling as
reviewed-but-unverified on real silicon.

---

## API

### `bool Radio::init()`

1. Configures RESET/RXEN as outputs, BUSY/DIO1 as inputs.
2. Toggles RESET and waits for BUSY to clear.
3. Sends `SetStandby(STDBY_RC)`.
4. Reads the chip status byte (`GetStatus`) to confirm the SPI link is alive.
5. Configures the LoRa modem: packet type, RF frequency, modulation params (SF/BW/CR),
   packet params, PA config, TX power, buffer base addresses, and DIO IRQ mask.

Returns `true` if the status byte looks sane (not `0x00`/`0xFF`) and configuration
completed.

```cpp
if (!radio.init()) {
    Serial.println("Radio init failed!");
}
```

---

### `bool Radio::send(const uint8_t* payload, uint8_t len)`

Writes the payload into the SX126x's internal buffer (`WriteBuffer`), patches the packet
length into `SetPacketParams`, clears pending IRQs, issues `SetTx` with a ~1 s timeout,
and polls `GetIrqStatus` until `TX_DONE` or `TIMEOUT`.

| Parameter | Description |
|-----------|-------------|
| `payload` | Pointer to byte array to transmit |
| `len` | Number of bytes in the array (max 255) |

Returns `true` if `TX_DONE` was observed before the poll loop's own 2 s deadline;
`false` otherwise.

```cpp
uint8_t data[] = {0x01, 0x02, 0x03};
radio.send(data, sizeof(data));
```

---

### `int Radio::receive(uint8_t* buffer, uint8_t maxLen, uint32_t timeoutMs)`

Issues `SetRx` with the given timeout, polls for `RX_DONE`, then reads the received
payload via `GetRxBufferStatus` + `ReadBuffer`.

| Parameter | Description |
|-----------|-------------|
| `buffer` | Destination for received bytes |
| `maxLen` | Capacity of `buffer` |
| `timeoutMs` | How long to listen before giving up |

Returns the number of bytes received, or `-1` on timeout/failure.

```cpp
uint8_t buf[64];
int n = radio.receive(buf, sizeof(buf), 2000);
if (n > 0) {
    Serial.printf("Received %d bytes\n", n);
}
```

---

### `bool Radio::present() const`

Returns whether `init()` successfully detected the radio (cached, no bus traffic).

---

## LoRa configuration

Configured from `include/config.hpp` during `init()`:

| Constant | Value | Meaning |
|----------|-------|---------|
| `RADIO_FREQ_HZ` | 868100000 | EU ISM band, 868.1 MHz |
| `RADIO_SF` | 7 | Spreading factor 7 (shortest range, fastest) |
| `RADIO_BW_KHZ` | 125 | LoRa bandwidth |
| `RADIO_CR` | 5 | Coding rate 4/5 |
| `RADIO_POWER_DBM` | 12 | Transmit power |

PA config is set for the SX1262 high-power path (up to +22 dBm), matching the
E22-900M22S module's rated output. To change frequency/SF/BW/CR/power, edit the
constants in `config.hpp` — `Radio::init()` reads them directly.

---

## SX126x command opcodes used

| Command | Opcode | Purpose |
|---------|--------|---------|
| `SetStandby` | `0x80` | Enter standby before configuration |
| `SetPacketType` | `0x8A` | Select LoRa |
| `SetRfFrequency` | `0x86` | Set carrier frequency |
| `SetModulationParams` | `0x8B` | SF / BW / CR |
| `SetPacketParams` | `0x8C` | Preamble, header type, payload length, CRC, IQ |
| `SetPaConfig` | `0x95` | Power amplifier configuration |
| `SetTxParams` | `0x8E` | TX power + ramp time |
| `SetBufferBaseAddress` | `0x8F` | TX/RX buffer offsets |
| `WriteBuffer` / `ReadBuffer` | `0x0E` / `0x1E` | Payload transfer |
| `SetDioIrqParams` | `0x08` | Route IRQ flags to DIO1 |
| `SetTx` / `SetRx` | `0x83` / `0x82` | Start transmit/receive with timeout |
| `GetIrqStatus` / `ClearIrqStatus` | `0x12` / `0x02` | Poll and clear IRQ flags |
| `GetRxBufferStatus` | `0x13` | Payload length + start address after RX |
| `GetStatus` | `0xC0` | Chip status byte (used as a presence check) |

---

## Range/airtime tradeoffs

| SF | Airtime (typical) | Range |
|----|-------------------|-------|
| SF7 | ~50 ms | shortest |
| SF9 | ~200 ms | medium |
| SF12 | ~2000 ms | longest |

CanSat flights typically use SF7 or SF9 to keep latency low during the descent phase.

---

## Notes

- **No physical switch:** the radio is on its own SPI2 bus, independent of the USB
  console UART — the serial monitor and radio work simultaneously.
- **Shared SPI2 bus:** the SD card is on the same physical bus with a separate CS line
  (`PIN_RADIO_CS` vs `PIN_SD_CS`). Both CS pins are held idle-high by `Board::init()`.
- **RXEN pin:** this board wires antenna-switch control to a discrete GPIO (`PIN_RADIO_RXEN`)
  rather than the SX126x's DIO2-as-RF-switch feature — the driver toggles it manually
  around `send()`/`receive()`.
- **Duty cycle:** the 868 MHz EU band enforces a 1% duty cycle. At SF7 with ~50 ms
  packets, the maximum safe transmission rate is roughly one packet per 5 seconds.
- **Initialisation order:** `Radio::init()` must be called after `Board::init()` because
  it uses the SPI2 bus that `Board::init()` sets up.
- **Regulator mode:** the driver does not call `SetRegulatorMode` and relies on the
  SX1262's default (LDO). If the E22-900M22S module has a populated DC-DC regulator,
  switching to `SetRegulatorMode(DCDC)` would reduce power draw — a good follow-up once
  real hardware is available to verify against.

---

## Global instance

```cpp
extern Radio radio;  // defined in radio.cpp
```

Include `radio.hpp` to use `radio` from any translation unit.
