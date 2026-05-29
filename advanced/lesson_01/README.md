# Lesson 1 — ESP-IDF Hello World & Project Structure

## Learning Objectives
- Understand that ESP-IDF uses `app_main()` instead of Arduino's `setup()` / `loop()`
- Use the structured logging macros: `ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE`
- Delay execution with `vTaskDelay(pdMS_TO_TICKS(ms))`

## Key Concepts

### Entry Point
In ESP-IDF there is no `setup()` or `loop()`. The firmware starts at `app_main()`, which runs in a FreeRTOS task automatically created by the scheduler. To repeat something, use a `while(true)` loop with a `vTaskDelay()` call inside.

### Logging
ESP-IDF provides log macros that include severity level, timestamp, and tag:
```c
ESP_LOGI(TAG, "Info message: value=%d", val);   // Info
ESP_LOGW(TAG, "Warning!");                       // Warning
ESP_LOGE(TAG, "Error: %s", esp_err_to_name(e)); // Error
```
The tag (e.g. `"Lesson1"`) is used to filter output and shows in the monitor.

### Delays
Never use busy-wait loops. Instead, yield the CPU back to the RTOS scheduler:
```c
vTaskDelay(pdMS_TO_TICKS(1000));  // wait 1000 ms
```

## Your Task
Open `src/main.cpp` and complete the three TODO comments. When done, flash and open the monitor:
```
pio run -t upload && pio device monitor
```
You should see lines like:
```
I (1234) Lesson1: Hello, CanSat! count=0
I (2234) Lesson1: Hello, CanSat! count=1
...
W (6234) Lesson1: Five seconds passed!
```

## Hardware
No external hardware needed for this lesson — only the USB serial connection.
