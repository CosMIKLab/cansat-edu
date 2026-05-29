/*
 * Lesson 4 – Solution: GPIO, Interrupts & Event Groups
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

static void IRAM_ATTR button_isr(void* arg) {
    BaseType_t higher_prio_woken = pdFALSE;
    xEventGroupSetBitsFromISR(s_evt_group, BTN_PRESSED_BIT, &higher_prio_woken);
    portYIELD_FROM_ISR(higher_prio_woken);
}

extern "C" void app_main(void) {
    s_evt_group = xEventGroupCreate();

    gpio_config_t led_cfg = {
        .pin_bit_mask = (1ULL << PIN_LED),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&led_cfg);
    gpio_set_level(PIN_LED, 0);

    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << PIN_BUTTON),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&btn_cfg);

    gpio_install_isr_service(0);
    gpio_isr_handler_add((gpio_num_t)PIN_BUTTON, button_isr, NULL);

    ESP_LOGI(TAG, "Ready — press the BOOT button to toggle the LED");

    bool led_state = false;
    while (true) {
        xEventGroupWaitBits(s_evt_group, BTN_PRESSED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        led_state = !led_state;
        gpio_set_level((gpio_num_t)PIN_LED, led_state ? 1 : 0);
        ESP_LOGI(TAG, "LED %s", led_state ? "ON" : "OFF");
    }
}
