/*
 * Lesson 5 – FreeRTOS Tasks & Priorities
 *
 * Learning objectives:
 *  - Create tasks with xTaskCreate(), choosing appropriate stack size and priority
 *  - Understand that each task has its own stack and runs independently
 *  - Use vTaskDelay() to yield to lower-priority tasks
 *
 * Your task:
 *  Create two sensor tasks: bme_task (reads BME280 every 2 s) and
 *  imu_task (reads LSM6DSOX every 500 ms).  Both print their data to
 *  the serial monitor.  The I2C bus is created in app_main() and passed
 *  to both tasks via a shared global.
 *
 *  WARNING: In this lesson there is NO mutex protecting the I2C bus.
 *           Both tasks use it — this is intentionally unsafe and sets up
 *           the problem solved in Lesson 7.
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

// Shared bus handle — initialised before tasks start
static i2c_master_bus_handle_t s_bus;

static void bme_task(void* arg) {
    // TODO: Initialise bme with s_bus. Log error and delete this task if it fails.
    // Hint: vTaskDelete(NULL) deletes the calling task.

    while (true) {
        float t, p, h;
        // TODO: Call bme.read() and log the values.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void imu_task(void* arg) {
    // TODO: Initialise imu with s_bus. Log warning and delete this task if it fails.

    while (true) {
        ImuData d;
        // TODO: Call imu.read() and log ax/ay/az.
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

    // TODO: Create bme_task with xTaskCreate().
    //   Stack: 4096 bytes, priority: 5, name: "bme_task"
    // Hint: xTaskCreate(function, "name", stack_words, arg, priority, handle_or_NULL)

    // TODO: Create imu_task with xTaskCreate().
    //   Stack: 4096 bytes, priority: 5, name: "imu_task"

    // app_main() must not return while tasks are running.
    // Deleting this task is the correct way to exit app_main after spawning tasks.
    vTaskDelete(NULL);
}
