/*
 * Lesson 3 – Solution: I2C Multi-Device & LSM6DSOX IMU
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

static const char* TAG = "Lesson3";

static BME280   bme;
static LSM6DSOX imu;

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
        ESP_LOGE(TAG, "BME280 init failed");
        return;
    }

    if (!imu.init(bus_handle)) {
        ESP_LOGW(TAG, "LSM6DSOX init failed — IMU readings will be skipped");
    }

    while (true) {
        float temp, press, hum;
        if (bme.read(temp, press, hum)) {
            ESP_LOGI(TAG, "BME280 — T:%.2f°C  P:%.2fhPa  H:%.1f%%",
                     temp, press, hum);
        }

        ImuData d;
        if (imu.read(d)) {
            ESP_LOGI(TAG, "IMU — ax:%.3f ay:%.3f az:%.3f  gx:%.2f gy:%.2f gz:%.2f",
                     d.ax, d.ay, d.az, d.gx, d.gy, d.gz);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
