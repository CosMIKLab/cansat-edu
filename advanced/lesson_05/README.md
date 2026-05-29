# Lesson 5 — FreeRTOS Tasks & Priorities

## Learning Objectives
- Create tasks with `xTaskCreate()` and choose stack size and priority
- Understand concurrent execution: tasks run independently and interleave
- Know that `app_main()` is itself a task — deleting it is correct when it's done

## Key Concepts

### xTaskCreate
```c
xTaskCreate(
    task_function,  // void (*)(void*)
    "task_name",    // name (used by debugger)
    4096,           // stack size in WORDS (not bytes; 4096 words = 16 KB)
    NULL,           // argument passed to function
    5,              // priority (0=lowest, configMAX_PRIORITIES-1=highest)
    NULL            // optional handle output
);
```

### Priorities
FreeRTOS is preemptive: a higher-priority task immediately preempts a lower one.  
- Timer daemon task runs at `configTIMER_TASK_PRIORITY` (default 1).
- Idle task runs at priority 0.
- Application tasks: use 1–10 for most use cases.
- Never make a task that busy-waits at high priority — it will starve everything else.

### Stack Sizing
Each task has a private stack. Overflowing it causes silent corruption or a crash.  
Use `uxTaskGetStackHighWaterMark(NULL)` to monitor how much stack a task actually uses.

### Ending app_main
```c
// Option 1: delete yourself
vTaskDelete(NULL);

// Option 2: loop forever (wastes 1 priority slot)
while (true) vTaskDelay(portMAX_DELAY);
```

## Build & Flash
```
pio run -t upload && pio device monitor
```
You will see two independent streams of log lines from `BME_Task` and `IMU_Task` interleaved.

> **Note:** This lesson intentionally has no mutex on the I2C bus. In practice, two tasks accessing I2C simultaneously can cause garbled data. Lesson 7 fixes this.
