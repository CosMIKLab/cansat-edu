/*
 * Lesson 1 – ESP-IDF Hello World & Project Structure
 *
 * Learning objectives:
 *  - Understand the ESP-IDF project layout (no Arduino setup()/loop())
 *  - Use ESP_LOGI / ESP_LOGW / ESP_LOGE for structured logging
 *  - Use vTaskDelay() for non-blocking delays inside app_main()
 *
 * Your task:
 *  Complete the TODOs below so the program prints "Hello, CanSat!"
 *  every second with a proper log tag, and a warning every 5 seconds.
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson1";

extern "C" void app_main(void) {
    int count = 0;

    while (true) {
        // TODO: Use ESP_LOGI to print "Hello, CanSat! count=%d" with the current count.
        // Hint: ESP_LOGI(TAG, "format string", args...);

        count++;

        // TODO: Every 5 iterations (when count % 5 == 0), use ESP_LOGW to
        // print "Five seconds passed!" as a warning.

        // TODO: Use vTaskDelay to wait 1000 milliseconds.
        // Hint: vTaskDelay(pdMS_TO_TICKS(ms));
    }
}
