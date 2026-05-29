/*
 * Lesson 4 – GPIO, Interrupts & Event Groups
 *
 * Learning objectives:
 *  - Configure GPIO pins as input (button) and output (LED)
 *  - Install an ISR service and attach a handler to a GPIO interrupt
 *  - Use an EventGroup to signal from an ISR to a task safely
 *
 * Your task:
 *  Configure GPIO_NUM_0 (BOOT button on most dev boards) as an input with
 *  a falling-edge interrupt.  On each press, set a bit in an event group.
 *  The main loop waits for that bit and toggles the LED on GPIO_NUM_2.
 *
 *  Important: ISR callbacks must be in IRAM and must only use ISR-safe APIs.
 */

#include "config.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char* TAG = "Lesson4";

#define BTN_PRESSED_BIT BIT0

static EventGroupHandle_t s_evt_group;

// ISR handler — called in interrupt context (IRAM, no heap, no float)
static void IRAM_ATTR button_isr(void* arg) {
    BaseType_t higher_prio_woken = pdFALSE;
    // TODO: Use xEventGroupSetBitsFromISR() to set BTN_PRESSED_BIT.
    //   Pass &higher_prio_woken as the last argument.
    //   Then call portYIELD_FROM_ISR(higher_prio_woken).
}

extern "C" void app_main(void) {
    // TODO: Create an event group and store it in s_evt_group.

    // TODO: Configure PIN_LED as push-pull output, initially low.
    //   Use gpio_config_t with .mode = GPIO_MODE_OUTPUT.

    // TODO: Configure PIN_BUTTON as input with pull-up, falling-edge interrupt.
    //   Use gpio_config_t with:
    //     .mode      = GPIO_MODE_INPUT
    //     .pull_up_en = GPIO_PULLUP_ENABLE
    //     .intr_type = GPIO_INTR_NEGEDGE

    // TODO: Install the global ISR service with gpio_install_isr_service(0).
    //   Then add the handler: gpio_isr_handler_add(PIN_BUTTON, button_isr, NULL).

    ESP_LOGI(TAG, "Ready — press the BOOT button to toggle the LED");

    bool led_state = false;
    while (true) {
        // TODO: Wait for BTN_PRESSED_BIT in the event group (block indefinitely).
        //   Use xEventGroupWaitBits() with xClearOnExit = pdTRUE.
        //   On return, toggle led_state and write it to PIN_LED with gpio_set_level().
        //   Log the new state.
    }
}
