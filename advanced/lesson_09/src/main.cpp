/*
 * Lesson 9 – SD Card & FAT Filesystem
 *
 * Learning objectives:
 *  - Mount a FAT-formatted SD card over SPI with esp_vfs_fat_sdspi_mount()
 *  - Use POSIX file I/O (fopen, fprintf, fflush, fclose) via the VFS layer
 *  - Implement session-based directory management (S001, S002, ...)
 *
 * Your task:
 *  Complete the TODOs in app_main() to:
 *    1. Mount the SD card
 *    2. Call Storage::init() which creates the session directory and files
 *    3. Log 10 dummy telemetry records and an event message
 *    4. Close the storage gracefully
 *
 *  The Storage driver in lib/storage/ handles the session logic; your job
 *  is to drive it from app_main().
 */

#include "config.h"
#include "storage.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson9";

extern "C" void app_main(void) {

    // TODO: Call Storage::init().
    //   If it returns false, log an error and return.

    // TODO: Log an event with Storage::event("Mission started");

    for (int i = 0; i < 10; i++) {
        TelemetryRecord rec = {
            .time_ms  = (uint32_t)(esp_timer_get_time() / 1000),
            .temp     = 20.0f + i * 0.5f,
            .pressure = 1013.25f - i * 0.1f,
            .humidity = 50.0f,
            .ax = 0.0f, .ay = 0.0f, .az = 1.0f,
            .gx = 0.0f, .gy = 0.0f, .gz = 0.0f,
        };

        // TODO: Call Storage::log(rec).
        //   If it returns false, log a warning.

        ESP_LOGI(TAG, "Logged record %d", i);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    // TODO: Call Storage::event("Mission complete") and then Storage::close().

    ESP_LOGI(TAG, "Done — remove SD card and check the files");

    while (true) vTaskDelay(portMAX_DELAY);
}
