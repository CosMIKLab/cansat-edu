# Getting Started — Beginner Framework

## Prerequisites

- [PlatformIO Core CLI](https://platformio.org/install/cli) — one-time install, handles compilation and flashing
- The `mecseksat` CLI — see install instructions in [`../download-guide.md`](../download-guide.md)
- A school access key (from your teacher or the MecsekSat team)
- USB cable to the CanSat board

Verify both are installed:
```bash
pio --version
mecseksat help
```

---

## First run

```bash
mecseksat login <your school key>
mecseksat get begginer/1
cd begginer/lesson_1
mecseksat run
```

`mecseksat get` downloads the lesson content and — for the begginer track — the
`cansat-edu-lib` framework this doc describes, placed as a sibling directory so
`platformio.ini`'s `lib_extra_dirs = ../../cansat-edu-lib` resolves without any extra
steps.

`mecseksat run` compiles `mission.cpp`, flashes it to the board, and opens the serial
monitor automatically.

Expected output at 115200 baud:
```
Sensors: BMP580 OK
Sensors: AHT20 OK
Sensors: TMP102 OK
Sensors: IMU OK
Sensors: GNSS OK
Radio: LoRa ready
SD: ready
Temp: 23.45 C
Pressure: 1013.25 hPa
Humidity: 48.70 %
...
```

---

## Editing your mission

Open `src/mission.cpp` in your editor. It is the **only file you need to change**.

```cpp
#include <cansat.h>

void mission_setup() {
    // called once at boot — connect WiFi here if needed
}

void mission_loop() {
    // called every 2 seconds — read sensors, send data
    float temp     = sensors.temperature();
    float pressure = sensors.pressure();
    float humidity = sensors.humidity();

    radio.send(temp, pressure, humidity);
    sd.log(temp, pressure, humidity);
}
```

After editing, run `mecseksat run` again to flash the new version.

---

## CLI reference

Run all commands from inside the downloaded lesson directory (`begginer/lesson_1/`, etc).

| Command | What it does |
|---------|-------------|
| `mecseksat run` | Compile → flash → open serial monitor |
| `mecseksat build` | Compile only (check for errors without flashing) |
| `mecseksat monitor` | Open serial monitor (board already flashed) |
| `mecseksat check` | Hardware diagnostic: test all sensors/SD/radio |
| `mecseksat update` | Download the latest version of this lesson (keeps `mission.cpp`) |
| `mecseksat new` | Reset `mission.cpp` to the original starter template |
| `mecseksat help` | Print command list |

---

## Flashing tips

- If upload fails: hold the BOOT button, press RESET, then re-run `mecseksat run`
- Upload speed is 115200 baud (set in `platformio.ini`)
- The board's native USB-Serial-JTAG handles reset/bootloader entry automatically —
  no external USB-to-serial adapter needed
- The board auto-resets after a successful flash and starts running your code immediately

---

## Reset to starter template

If your `mission.cpp` is broken and you want to start fresh:

```bash
mecseksat new
```

This copies the pristine `mission_template.cpp` back to `src/mission.cpp`.

---

## See also

| File | Contents |
|------|----------|
| [api.md](api.md) | Full API reference for `sensors`, `radio`, `wifi`, `sd`, `led` |
| [overview.md](overview.md) | Framework architecture, downloaded-lesson layout |
