/*
 * Lesson 7 – FreeRTOS Mutexes & Shared Resources
 *
 * Learning objectives:
 *  - Protect a shared resource (I2C bus) with a mutex
 *  - Use xSemaphoreCreateMutex(), xSemaphoreTake(), xSemaphoreGive()
 *  - Understand why concurrent I2C access without a mutex corrupts data
 *
 * Context:
 *  Lesson 5 created two tasks that both access the I2C bus without
 *  synchronisation.  This lesson adds a mutex so that only one task
 *  accesses the bus at a time.
 *
 * Your task:
 *  Add a mutex (s_i2c_mutex) and wrap EVERY call to bme.read() and
 *  imu.read() with xSemaphoreTake / xSemaphoreGive pairs.
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG_BME = "BME_Task";
static const char* TAG_IMU = "IMU_Task";

static BME280   bme;
static LSM6DSOX imu;
static i2c_master_bus_handle_t s_bus;

// TODO: Declare a SemaphoreHandle_t named s_i2c_mutex.

static void bme_task(void* arg) {
    // NOTE: init() is called before the other task starts, so no mutex needed here.
    if (!bme.init(s_bus)) { ESP_LOGE(TAG_BME, "Init failed"); vTaskDelete(NULL); }

    while (true) {
        float t, p, h;

        // TODO: Take s_i2c_mutex (timeout: pdMS_TO_TICKS(500)).
        //   On success: call bme.read(t, p, h), then Give the mutex, then log.
        //   On failure (timeout): log "Mutex timeout".

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void imu_task(void* arg) {
    vTaskDelay(pdMS_TO_TICKS(100));  // let bme_task init first
    if (!imu.init(s_bus)) { ESP_LOGW(TAG_IMU, "Init failed"); vTaskDelete(NULL); }

    while (true) {
        ImuData d;

        // TODO: Take s_i2c_mutex (timeout: pdMS_TO_TICKS(500)).
        //   On success: call imu.read(d), then Give the mutex, then log ax/ay/az.
        //   On failure: log "Mutex timeout".

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

    // TODO: Create s_i2c_mutex with xSemaphoreCreateMutex().

    xTaskCreate(bme_task, "bme_task", 4096, NULL, 5, NULL);
    xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);

    vTaskDelete(NULL);
}
