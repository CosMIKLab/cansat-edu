# Lesson 8 — FreeRTOS Software Timers

## Learning Objectives
- Create one-shot and auto-reload software timers with `xTimerCreate()`
- Understand the timer daemon task and its constraints
- Pass data from a timer callback to a regular task via a queue

## Key Concepts

### Software Timer Creation
```c
TimerHandle_t t = xTimerCreate(
    "my_timer",          // name (debugging only)
    pdMS_TO_TICKS(2000), // period in ticks
    pdTRUE,              // pdTRUE = auto-reload, pdFALSE = one-shot
    NULL,                // timer ID (arbitrary pointer)
    callback_fn          // void (*)(TimerHandle_t)
);
xTimerStart(t, portMAX_DELAY);
```

### Timer Callback Rules
Callbacks run in the **timer daemon task** context (priority `configTIMER_TASK_PRIORITY`, default 1):
- Do **not** block (no `vTaskDelay`, no blocking receive)
- Do **not** call `vTaskDelete` or `vTaskSuspend`
- Keep them short — long callbacks delay other timers
- Use non-blocking queue sends (`xQueueSend(q, &item, 0)`)

### One-shot vs. Auto-reload
| | One-shot | Auto-reload |
|---|---|---|
| Repeats | No | Yes, every period |
| Use case | Debounce, watchdog kick | Periodic sampling |
| Restart | `xTimerStart()` again | Automatic |

### Timer Management
```c
xTimerStop(t, 0);     // stop timer
xTimerReset(t, 0);    // restart from now
xTimerChangePeriod(t, pdMS_TO_TICKS(500), 0);  // change period
```

## Build & Flash
```
pio run -t upload && pio device monitor
```
Expected: BME280 readings appear every 2 seconds, printed by `logger_task`.
