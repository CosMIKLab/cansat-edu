/*
 * Lesson 9 – Solution: SD Card & FAT Filesystem
 */

#include "config.h"
#include "storage.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson9";

extern "C" void app_main(void) {
    if (!Storage::init()) {
        ESP_LOGE(TAG, "Storage init failed — check SD card wiring");
        return;
    }

    Storage::event("Mission started");

    for (int i = 0; i < 10; i++) {
        TelemetryRecord rec = {
            .time_ms  = (uint32_t)(esp_timer_get_time() / 1000),
            .temp     = 20.0f + i * 0.5f,
            .pressure = 1013.25f - i * 0.1f,
            .humidity = 50.0f,
            .ax = 0.0f, .ay = 0.0f, .az = 1.0f,
            .gx = 0.0f, .gy = 0.0f, .gz = 0.0f,
        };

        if (!Storage::log(rec)) {
            ESP_LOGW(TAG, "Log failed for record %d", i);
        }

        ESP_LOGI(TAG, "Logged record %d", i);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    Storage::event("Mission complete");
    Storage::close();

    ESP_LOGI(TAG, "Done — remove SD card and check the files");

    while (true) vTaskDelay(portMAX_DELAY);
}
