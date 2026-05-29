/*
 * Lesson 11 – Solution: Power Management & Deep Sleep
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson11";

RTC_DATA_ATTR static uint32_t boot_count = 0;

#define SLEEP_DURATION_US (10ULL * 1000000ULL)

extern "C" void app_main(void) {
    boot_count++;

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        ESP_LOGI(TAG, "Woke from deep sleep (boot #%lu)", (unsigned long)boot_count);
    } else {
        ESP_LOGI(TAG, "First boot (power-on)");
    }

    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_cfg = {
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .i2c_port          = I2C_NUM_0,
        .sda_io_num        = PIN_SDA,
        .scl_io_num        = PIN_SCL,
        .glitch_ignore_cnt = 7,
        .flags             = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));

    BME280 bme;
    if (!bme.init(bus)) {
        ESP_LOGE(TAG, "BME280 init failed");
    } else {
        float t, p, h;
        if (bme.read(t, p, h)) {
            ESP_LOGI(TAG, "T:%.2f°C  P:%.2fhPa  H:%.1f%%", t, p, h);
        }
    }

    ESP_LOGI(TAG, "Sleeping for 10 s...");
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
    esp_deep_sleep_start();
}
