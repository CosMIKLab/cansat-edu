# Lesson 4 — GPIO, Interrupts & Event Groups

## Learning Objectives
- Configure GPIO pins for input (with pull-up) and output
- Write and install an ISR handler using `gpio_isr_handler_add()`
- Safely communicate from an ISR to a task using an `EventGroupHandle_t`

## Key Concepts

### GPIO Configuration
```c
gpio_config_t cfg = {
    .pin_bit_mask = (1ULL << GPIO_NUM_2),  // bitmask of pins
    .mode         = GPIO_MODE_OUTPUT,
    .pull_up_en   = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type    = GPIO_INTR_DISABLE,
};
gpio_config(&cfg);
gpio_set_level(GPIO_NUM_2, 1);  // set high
```

### ISR Rules
An ISR runs in interrupt context: no heap allocation, no blocking, no float.  
Only use `FromISR`-suffixed FreeRTOS functions:
```c
static void IRAM_ATTR my_isr(void* arg) {
    BaseType_t woken = pdFALSE;
    xEventGroupSetBitsFromISR(group, BIT0, &woken);
    portYIELD_FROM_ISR(woken);  // yield if a higher-priority task was unblocked
}
```
`IRAM_ATTR` places the function in fast RAM so it can execute even when flash is being accessed.

### Event Groups
```c
EventGroupHandle_t grp = xEventGroupCreate();

// In ISR (set bits):
xEventGroupSetBitsFromISR(grp, BIT0, &woken);

// In task (wait for bits):
EventBits_t bits = xEventGroupWaitBits(
    grp,        // group handle
    BIT0,       // bits to wait for
    pdTRUE,     // clear on exit
    pdFALSE,    // wait for ANY (not all) bits
    portMAX_DELAY
);
```

## Hardware
| Signal  | ESP32-S3 Pin |
|---------|-------------|
| LED     | GPIO 2 (built-in on many boards) |
| Button  | GPIO 0 (BOOT button on dev boards) |

## Build & Flash
```
pio run -t upload && pio device monitor
```
Press the BOOT button — the LED should toggle and the monitor should print the new state.
