/*
 * Lesson 11 – Power Management & Deep Sleep
 *
 * Learning objectives:
 *  - Put the ESP32-S3 into deep sleep with esp_deep_sleep_start()
 *  - Wake after a fixed interval with esp_sleep_enable_timer_wakeup()
 *  - Preserve state across sleep cycles using RTC_DATA_ATTR variables
 *
 * Behaviour:
 *  On each boot/wake:
 *    1. Read BME280
 *    2. Print the values and the boot count
 *    3. Deep sleep for SLEEP_DURATION_US microseconds
 *
 *  Deep sleep: CPU off, RAM off, only RTC domain alive → ~10 µA.
 *  Waking restarts app_main() from the top (not from where sleep was entered).
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson11";

// RTC memory survives deep sleep; normal RAM does not.
// TODO: Declare a static uint32_t named boot_count with RTC_DATA_ATTR prefix.
// Example: RTC_DATA_ATTR static uint32_t my_var = 0;

#define SLEEP_DURATION_US (10ULL * 1000000ULL)  // 10 seconds

extern "C" void app_main(void) {

    // TODO: Increment boot_count and log the boot count and wakeup cause.
    //   Use esp_sleep_get_wakeup_cause() to detect timer vs. power-on wakeup.
    //   ESP_SLEEP_WAKEUP_TIMER means we woke from deep sleep.

    // Initialise I2C and BME280
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

    // TODO: Configure a timer wakeup source: esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US)
    // TODO: Enter deep sleep: esp_deep_sleep_start()
    //   (Nothing after this line runs — deep sleep resets the CPU)
}
