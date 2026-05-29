# Lesson 11 — Power Management & Deep Sleep

## Learning Objectives
- Understand ESP32-S3 power modes: active, light sleep, deep sleep
- Enter deep sleep with a timer wakeup source
- Preserve state across sleep cycles using `RTC_DATA_ATTR`

## Key Concepts

### Power Modes
| Mode | Current | RAM | CPU | Wake Sources |
|------|---------|-----|-----|-------------|
| Active | ~80 mA | On | Running | — |
| Light sleep | ~800 µA | Retained | Paused | Timer, GPIO, UART |
| Deep sleep | ~10 µA | **Lost** | Off | Timer, GPIO, ULP |

### Deep Sleep API
```c
// Configure wakeup source (must do before sleeping)
esp_sleep_enable_timer_wakeup(10 * 1000000ULL);  // 10 seconds in µs

// Enter deep sleep (does not return — CPU resets on wake)
esp_deep_sleep_start();
```

### RTC Memory
Deep sleep shuts down all SRAM. Variables tagged `RTC_DATA_ATTR` are stored in the small RTC memory (8 KB) that remains powered:
```c
RTC_DATA_ATTR static uint32_t boot_count = 0;  // survives deep sleep
```
Initialised to 0 at power-on; preserved across all subsequent deep sleeps.

### Detecting Wakeup Cause
```c
esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
switch (cause) {
    case ESP_SLEEP_WAKEUP_TIMER:  // timer fired
    case ESP_SLEEP_WAKEUP_GPIO:   // GPIO edge
    case ESP_SLEEP_WAKEUP_UNDEFINED: // power-on or reset
}
```

### After Waking
The CPU re-runs `app_main()` from the top — exactly like a normal boot but with RTC memory intact.

## Build & Flash
```
pio run -t upload && pio device monitor
```
You will see the boot count increase every 10 seconds. The current draw between prints drops to ~10 µA (not measurable on USB, but visible with a current probe).
