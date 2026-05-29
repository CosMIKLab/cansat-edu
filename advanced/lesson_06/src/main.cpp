/*
 * Lesson 6 – FreeRTOS Queues (Sensor Pipeline)
 *
 * Learning objectives:
 *  - Create a queue with xQueueCreate() to pass structs between tasks
 *  - Use xQueueSend() (producer) and xQueueReceive() (consumer)
 *  - Decouple sensor reading from logging/processing
 *
 * Architecture:
 *   sensor_task  →  [queue]  →  logger_task
 *
 * Your task:
 *  Create a queue that holds SensorReading structs.  sensor_task fills
 *  the struct and sends it.  logger_task receives and prints it.
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char* TAG_SENSOR = "Sensor";
static const char* TAG_LOGGER = "Logger";

struct SensorReading {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
};

static BME280   bme;
static LSM6DSOX imu;
static i2c_master_bus_handle_t s_bus;

// TODO: Declare a QueueHandle_t named s_queue.

static void sensor_task(void* arg) {
    if (!bme.init(s_bus)) { ESP_LOGE(TAG_SENSOR, "BME280 init failed"); vTaskDelete(NULL); }
    if (!imu.init(s_bus)) { ESP_LOGW(TAG_SENSOR, "LSM6DSOX init failed"); }

    while (true) {
        SensorReading reading = {};
        reading.time_ms = (uint32_t)(esp_timer_get_time() / 1000);

        float t, p, h;
        if (bme.read(t, p, h)) {
            reading.temp = t; reading.pressure = p; reading.humidity = h;
        }
        ImuData d;
        if (imu.read(d)) {
            reading.ax = d.ax; reading.ay = d.ay; reading.az = d.az;
        }

        // TODO: Send 'reading' to s_queue using xQueueSend().
        //   Use a timeout of 0 (don't block if queue is full).
        //   If it fails, log a warning "Queue full, dropping sample".

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void logger_task(void* arg) {
    while (true) {
        SensorReading reading;
        // TODO: Receive a reading from s_queue using xQueueReceive().
        //   Block indefinitely (portMAX_DELAY) until one arrives.
        //   Then log all fields.
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

    // TODO: Create s_queue to hold 8 SensorReading items.
    //   Hint: xQueueCreate(length, item_size)

    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(logger_task, "logger", 4096, NULL, 4, NULL);

    vTaskDelete(NULL);
}
