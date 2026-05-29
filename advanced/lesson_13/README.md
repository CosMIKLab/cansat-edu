# Lesson 13 — Full Telemetry Pipeline

## Learning Objectives
- Design a multi-task pipeline with queue fan-out to decouple producers and consumers
- Monitor system health (stack usage, queue depth) with a supervisor task
- Assign task priorities to reflect real-time requirements

## Architecture

```
sensor_task (prio 5) ──┬──► [q_sd    depth=8] ──► sd_task    (prio 4)
                       └──► [q_radio depth=4] ──► radio_task (prio 3)

monitor_task (prio 2) — prints watermarks every 10 s
```

**Priority assignment rationale:**
- `sensor_task` = highest: must sample on schedule
- `sd_task` = next: data loss is more damaging than late radio TX
- `radio_task` = lower: EU duty cycle forces a 5 s gap anyway
- `monitor_task` = lowest: diagnostic only, can be delayed

## Key Patterns

### Queue Fan-out
One producer sends to two queues independently:
```c
xQueueSend(q_sd,    &reading, 0);
xQueueSend(q_radio, &reading, 0);
```
Each consumer reads from its own queue without blocking the other.

### Stack Watermark Monitoring
```c
UBaseType_t free_words = uxTaskGetStackHighWaterMark(task_handle);
// free_words = minimum remaining stack space in words
// If this approaches 0, increase the stack size
```
Rule of thumb: keep at least 512 words of headroom.

### Queue Depth Monitoring
```c
UBaseType_t waiting = uxQueueMessagesWaiting(queue_handle);
// If consistently near maximum depth, consumers are too slow
```

## Binary Packet Format (radio_task)
4 floats (temp, pressure, humidity, az) packed as 16 raw bytes:
```
[4B float temp][4B float pressure][4B float humidity][4B float az]
```
On the receiver, parse with `memcpy(&f, &buf[offset], 4)`.

## Build & Flash
```
pio run -t upload && pio device monitor
```
Monitor shows four interleaved log streams. Check SD card for telem.csv and verify the radio TX log entries.
