/*
 * Lesson 8 – Solution: FreeRTOS Software Timers
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
static QueueHandle_t     s_queue;
static SemaphoreHandle_t s_i2c_mutex;

static void timer_callback(TimerHandle_t xTimer) {
    BmeReading r = {};
    if (xSemaphoreTake(s_i2c_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        bme.read(r.temp, r.pressure, r.humidity);
        xSemaphoreGive(s_i2c_mutex);
    }
    xQueueSend(s_queue, &r, 0);
}

static void logger_task(void* arg) {
    while (true) {
        BmeReading r;
        xQueueReceive(s_queue, &r, portMAX_DELAY);
        ESP_LOGI(TAG, "T:%.2f°C  P:%.2fhPa  H:%.1f%%", r.temp, r.pressure, r.humidity);
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

    s_queue     = xQueueCreate(4, sizeof(BmeReading));
    s_i2c_mutex = xSemaphoreCreateMutex();

    TimerHandle_t timer = xTimerCreate(
        "bme_timer",
        pdMS_TO_TICKS(2000),
        pdTRUE,           // auto-reload
        NULL,
        timer_callback
    );
    xTimerStart(timer, portMAX_DELAY);

    xTaskCreate(logger_task, "logger", 4096, NULL, 4, NULL);

    vTaskDelete(NULL);
}
