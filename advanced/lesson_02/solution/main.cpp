/*
 * Lesson 2 – Solution: I2C Driver & BME280 Environmental Sensor
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

static const char* TAG = "Lesson2";

static BME280 bme;

extern "C" void app_main(void) {
    i2c_master_bus_handle_t bus_handle;

    i2c_master_bus_config_t bus_cfg = {
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .i2c_port          = I2C_NUM_0,
        .sda_io_num        = PIN_SDA,
        .scl_io_num        = PIN_SCL,
        .glitch_ignore_cnt = 7,
        .flags             = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    if (!bme.init(bus_handle)) {
        ESP_LOGE(TAG, "BME280 init failed — check wiring");
        return;
    }

    while (true) {
        float temp, press, hum;
        if (bme.read(temp, press, hum)) {
            ESP_LOGI(TAG, "Temp: %.2f °C  Pressure: %.2f hPa  Humidity: %.1f %%",
                     temp, press, hum);
        } else {
            ESP_LOGW(TAG, "BME280 read failed");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
