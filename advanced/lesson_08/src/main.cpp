/*
 * Lesson 8 – FreeRTOS Software Timers
 *
 * Learning objectives:
 *  - Create a periodic software timer with xTimerCreate()
 *  - Understand the difference between one-shot and auto-reload timers
 *  - Pass data from a timer callback to a task via a queue
 *
 * Architecture:
 *   [SW Timer callback]  →  [queue]  →  logger_task
 *
 * Your task:
 *  Create an auto-reload software timer that fires every 2 seconds.
 *  In the callback, read the BME280 (guarded by a mutex) and push the
 *  result to a queue.  logger_task prints each reading.
 *
 *  Note: timer callbacks run in the timer daemon task context.
 *  They must not block for long periods.
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static const char* TAG = "Lesson8";

struct BmeReading { float temp, pressure, humidity; };

static BME280 bme;
static i2c_master_bus_handle_t s_bus;

// TODO: Declare QueueHandle_t s_queue and SemaphoreHandle_t s_i2c_mutex.

static void timer_callback(TimerHandle_t xTimer) {
    BmeReading r = {};
    // TODO: Take s_i2c_mutex (timeout 10 ms — timer callbacks must be fast).
    //   Call bme.read(r.temp, r.pressure, r.humidity).
    //   Give the mutex.
    //   Send r to s_queue (non-blocking, timeout 0).
}

static void logger_task(void* arg) {
    while (true) {
        BmeReading r;
        // TODO: Receive from s_queue (block indefinitely) and log all three values.
    }
}

extern "C" void app_main(void) {
    i2c_master_bus_config_t bus_cfg = {
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .i2c_port          = I2C_NUM_0,
        .sda_io_num        = PIN_SDA,
        .scl_io_num        = PIN_SCL,
        .glitch_ignore_cnt = 7,
        .flags             = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_bus));

    if (!bme.init(s_bus)) { ESP_LOGE(TAG, "BME280 init failed"); return; }

    // TODO: Create s_queue (depth 4) and s_i2c_mutex.

    // TODO: Create a 2000 ms auto-reload timer named "bme_timer" using xTimerCreate().
    //   Pass timer_callback as the callback.
    //   Start the timer with xTimerStart().

    xTaskCreate(logger_task, "logger", 4096, NULL, 4, NULL);

    vTaskDelete(NULL);
}
