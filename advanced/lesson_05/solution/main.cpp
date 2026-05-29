/*
 * Lesson 5 – Solution: FreeRTOS Tasks & Priorities
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG_BME = "BME_Task";
static const char* TAG_IMU = "IMU_Task";

static BME280   bme;
static LSM6DSOX imu;
static i2c_master_bus_handle_t s_bus;

static void bme_task(void* arg) {
    if (!bme.init(s_bus)) {
        ESP_LOGE(TAG_BME, "Init failed");
        vTaskDelete(NULL);
    }
    while (true) {
        float t, p, h;
        if (bme.read(t, p, h)) {
            ESP_LOGI(TAG_BME, "T:%.2f°C  P:%.2fhPa  H:%.1f%%", t, p, h);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void imu_task(void* arg) {
    if (!imu.init(s_bus)) {
        ESP_LOGW(TAG_IMU, "Init failed");
        vTaskDelete(NULL);
    }
    while (true) {
        ImuData d;
        if (imu.read(d)) {
            ESP_LOGI(TAG_IMU, "ax:%.3f ay:%.3f az:%.3f", d.ax, d.ay, d.az);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
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

    xTaskCreate(bme_task, "bme_task", 4096, NULL, 5, NULL);
    xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);

    vTaskDelete(NULL);
}
