# Lesson 14 — Capstone: Full CanSat Mission

## Learning Objectives
- Load runtime configuration from NVS to avoid hard-coded values
- Subscribe tasks to the hardware task watchdog for automatic crash recovery
- Implement graceful sensor error handling without aborting the mission
- Produce a complete, production-quality embedded firmware

## System Overview

```
Boot → NVS config → I2C init → queues/mutex → tasks

sensor_task (prio 5) ──┬──► [q_sd    depth=16] ──► sd_task    (prio 4)
                       └──► [q_radio depth=8 ] ──► radio_task (prio 3, conditional)

wdt_task    (prio 6) — hardware WDT heartbeat
```

## Key Concepts

### NVS Configuration
```c
nvs_handle_t h;
nvs_open("mission", NVS_READONLY, &h);

uint32_t val;
nvs_get_u32(h, "tx_ms", &val);   // read key "tx_ms" into val
nvs_close(h);
```
Write defaults at the factory or from a provisioning tool. If no NVS partition exists, fall back to compiled defaults.

### Hardware Task Watchdog
The `esp_task_wdt` monitors subscribed tasks. If a task stops resetting it within the timeout, the watchdog fires a panic and reboots:
```c
// Register current task
esp_task_wdt_add(NULL);

// In the task loop (must happen at least every timeout_ms)
esp_task_wdt_reset();

// Configure
esp_task_wdt_config_t cfg = { .timeout_ms = 30000, .trigger_panic = true };
esp_task_wdt_reconfigure(&cfg);
```

### Error Flags
Each telemetry packet carries an `error_flags` byte:
- Bit 0: BME280 has failed 3 times consecutively
- Bit 1: LSM6DSOX has failed 3 times consecutively

The ground station or SD card analysis can detect partial sensor failures without losing the entire mission.

### Compact Radio Payload (17 bytes)
```c
struct __attribute__((packed)) Payload {
    uint32_t time_ms;   // 4 B
    float temp;         // 4 B
    float pressure;     // 4 B
    float humidity;     // 4 B
    float az;           // 4 B
    uint8_t err;        // 1 B
};                      // = 21 B total
```
`__attribute__((packed))` prevents compiler padding so `sizeof` matches the wire format.

## Writing NVS Config (optional)
Use `idf.py menuconfig` → Component config → NVS or write a one-time provisioning sketch:
```c
nvs_handle_t h;
nvs_open("mission", NVS_READWRITE, &h);
nvs_set_u32(h, "tx_ms",  5000);
nvs_set_u32(h, "log_ms", 1000);
nvs_set_u8(h,  "radio_en", 1);
nvs_commit(h);
nvs_close(h);
```

## Build & Flash
```
pio run -t upload && pio device monitor
```
Verify:
1. Serial shows telemetry every `log_ms` milliseconds
2. SD card contains `S00X/telem.csv` with all columns
3. Radio log shows hex packets at `tx_ms` intervals
4. If you pull a sensor connector, `error_flags` in the log changes from 0x00

## Congratulations
You have implemented a complete FreeRTOS-based CanSat mission firmware using:
- ESP-IDF I2C driver, UART driver, SPI/FAT SD, WiFi, NVS
- FreeRTOS tasks, queues, mutexes, software timers, event groups
- Hardware watchdog, deep sleep, power management
- Structured logging, error recovery, and compact radio encoding
