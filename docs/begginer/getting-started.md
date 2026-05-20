# Getting Started — Beginner Framework

## Prerequisites

- [PlatformIO Core CLI](https://platformio.org/install/cli) — one-time install, handles compilation and flashing
- USB cable to the CanSat board
- Any text editor (VS Code recommended)

Verify PlatformIO is installed:
```bash
pio --version
```

---

## First run

```bash
cd begginer
./cansat run        # Mac / Linux
cansat run          # Windows CMD
```

This compiles the starter `mission.cpp`, flashes it to the board, and opens the serial monitor automatically.

Expected output at 115200 baud:
```
Sensors: BME280 OK
Sensors: IMU OK
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

After editing, run `./cansat run` again to flash the new version.

---

## CLI reference

Run all commands from inside the `begginer/` folder.

| Command | What it does |
|---------|-------------|
| `./cansat run` | Compile → flash → open serial monitor |
| `./cansat build` | Compile only (check for errors without flashing) |
| `./cansat monitor` | Open serial monitor (board already flashed) |
| `./cansat new` | Reset `mission.cpp` to the original starter template |
| `./cansat help` | Print command list |

> **Physical switch:** Set the board's UART switch to **USB** before running the monitor or flashing. Set it to **Radio** before a flight so AT commands reach the RN2483.

---

## Flashing tips

- If upload fails: hold the BOOT button, press RESET, then re-run `./cansat run`
- Upload speed is 115200 baud (set in `platformio.ini`)
- The board auto-resets after a successful flash and starts running your code immediately

---

## Reset to starter template

If your `mission.cpp` is broken and you want to start fresh:

```bash
./cansat new       # Mac/Linux
cansat new         # Windows
```

This copies the pristine `mission_template.cpp` back to `src/mission.cpp`.
