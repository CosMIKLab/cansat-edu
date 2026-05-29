/*
 * Lesson 10 – Solution: UART & LoRa Radio (RN2483)
 */

#include "config.h"
#include "radio.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson10";

static Radio radio;

extern "C" void app_main(void) {
    if (!radio.init()) {
        ESP_LOGE(TAG, "Radio init failed");
        return;
    }

    const uint8_t payload[] = {'C', 'A', 'N', 'S', 'A', 'T'};
    int packet_count = 0;

    while (true) {
        ESP_LOGI(TAG, "Sending packet #%d", packet_count++);

        if (!radio.send(payload, sizeof(payload))) {
            ESP_LOGW(TAG, "Radio send failed");
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
