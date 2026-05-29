/*
 * Lesson 1 – Solution: ESP-IDF Hello World & Project Structure
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson1";

extern "C" void app_main(void) {
    int count = 0;

    while (true) {
        ESP_LOGI(TAG, "Hello, CanSat! count=%d", count);
        count++;

        if (count % 5 == 0) {
            ESP_LOGW(TAG, "Five seconds passed!");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
