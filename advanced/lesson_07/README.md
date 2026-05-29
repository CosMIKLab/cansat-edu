# Lesson 7 — FreeRTOS Mutexes & Shared Resources

## Learning Objectives
- Identify when a resource needs protection (shared I2C bus)
- Create and use a mutex with `xSemaphoreCreateMutex()`
- Apply the take/give pattern correctly (always give after take, even on error paths)

## Key Concepts

### Why Mutual Exclusion?
The I2C master driver is not re-entrant. If two tasks issue a transaction at the same time, the underlying registers get corrupted and sensors return garbage data or hang. A mutex ensures only one task holds the bus at a time.

### Mutex Pattern
```c
SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

// In any task that uses the shared resource:
if (xSemaphoreTake(mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
    // — critical section —
    use_shared_resource();
    // ——————————————
    xSemaphoreGive(mutex);
} else {
    ESP_LOGW(TAG, "Mutex timeout — resource busy for > 500 ms");
}
```
**Rule:** Every `Take` must be paired with a `Give`, on every code path, including error exits.

### Mutex vs. Binary Semaphore
| | Mutex | Binary Semaphore |
|---|---|---|
| Priority inheritance | Yes (FreeRTOS mutex) | No |
| Use case | Mutual exclusion | Signalling/synchronisation |
| ISR safe | No | Yes (if created with `xSemaphoreCreateBinary`) |

Use a **mutex** whenever protecting a shared resource between tasks.

### Priority Inversion
If a low-priority task holds the mutex and a high-priority task tries to take it, the high-priority task blocks. FreeRTOS mutexes implement priority inheritance: the low-priority task temporarily runs at the higher priority until it releases the mutex, preventing indefinite blocking.

## Build & Flash
```
pio run -t upload && pio device monitor
```
You should see safe interleaving of BME280 and IMU data with no corruption.
