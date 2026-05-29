# Lesson 6 — FreeRTOS Queues (Sensor Pipeline)

## Learning Objectives
- Decouple producers (sensor tasks) from consumers (logger task) using a queue
- Understand blocking vs. non-blocking queue operations
- Design pipeline architectures with bounded buffers

## Key Concepts

### Queue Lifecycle
```c
// Create (once, before tasks start)
QueueHandle_t q = xQueueCreate(8, sizeof(MyStruct));

// Producer task — non-blocking send
if (xQueueSend(q, &item, 0) != pdTRUE) {
    // queue full — drop or wait
}

// Consumer task — blocking receive
MyStruct item;
xQueueReceive(q, &item, portMAX_DELAY);  // block until data arrives
```

### Why a Queue?
- The sensor task does not need to know about the logger task
- The logger task can process at its own rate; the queue absorbs bursts
- If the queue fills up, the producer drops data rather than blocking — this keeps the sensor on schedule

### Queue Depth Trade-offs
| Depth | Memory | Behaviour |
|-------|--------|-----------|
| 1     | minimal | drop immediately if consumer is slow |
| 8–16  | moderate | buffer a few seconds of bursts |
| Large | wastes RAM | delays detection of real overruns |

### Data Ownership
When you call `xQueueSend()`, FreeRTOS **copies** the struct into the queue. There is no shared pointer. Both tasks can safely use their own local copies.

## Build & Flash
```
pio run -t upload && pio device monitor
```
Expected: `Logger` prints each reading as it arrives; readings are numbered by timestamp.
