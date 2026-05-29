/*
 * Lesson 13 – Solution: Full Telemetry Pipeline
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "radio.hpp"
#include "storage.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <string.h>

static const char* TAG_SENSOR  = "Sensor";
static const char* TAG_SD      = "SD";
static const char* TAG_RADIO   = "Radio";
static const char* TAG_MONITOR = "Monitor";

struct SensorReading {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
    float gx, gy, gz;
};

static BME280   bme;
static LSM6DSOX imu;
static Radio    radio;

static i2c_master_bus_handle_t s_bus;
static SemaphoreHandle_t       s_i2c_mutex;
static QueueHandle_t           q_sd;
static QueueHandle_t           q_radio;

static TaskHandle_t h_sensor, h_sd, h_radio;

static void sensor_task(void* arg) {
    xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);
    bool bme_ok = bme.init(s_bus);
    bool imu_ok = imu.init(s_bus);
    xSemaphoreGive(s_i2c_mutex);

    if (!bme_ok) { ESP_LOGE(TAG_SENSOR, "BME280 init failed"); vTaskDelete(NULL); }
    if (!imu_ok)   ESP_LOGW(TAG_SENSOR, "LSM6DSOX init failed — continuing");

    while (true) {
        SensorReading r = {};
        r.time_ms = (uint32_t)(esp_timer_get_time() / 1000);

        xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);
        float t, p, h;
        if (bme.read(t, p, h)) { r.temp = t; r.pressure = p; r.humidity = h; }
        ImuData d;
        if (imu.read(d)) {
            r.ax = d.ax; r.ay = d.ay; r.az = d.az;
            r.gx = d.gx; r.gy = d.gy; r.gz = d.gz;
        }
        xSemaphoreGive(s_i2c_mutex);

        if (xQueueSend(q_sd, &r, 0) != pdTRUE)
            ESP_LOGW(TAG_SENSOR, "SD queue full");
        if (xQueueSend(q_radio, &r, 0) != pdTRUE)
            ESP_LOGW(TAG_SENSOR, "Radio queue full");

        ESP_LOGI(TAG_SENSOR, "T:%.2f P:%.2f H:%.1f | ax:%.3f",
                 r.temp, r.pressure, r.humidity, r.ax);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void sd_task(void* arg) {
    if (!Storage::init()) { ESP_LOGE(TAG_SD, "Storage init failed"); vTaskDelete(NULL); }
    Storage::event("SD task started");

    while (true) {
        SensorReading r;
        xQueueReceive(q_sd, &r, portMAX_DELAY);

        TelemetryRecord rec = {
            .time_ms  = r.time_ms,
            .temp     = r.temp,
            .pressure = r.pressure,
            .humidity = r.humidity,
            .ax = r.ax, .ay = r.ay, .az = r.az,
            .gx = r.gx, .gy = r.gy, .gz = r.gz,
        };
        if (!Storage::log(rec)) {
            ESP_LOGW(TAG_SD, "Storage::log failed");
        }
    }
}

static void radio_task(void* arg) {
    if (!radio.init()) { ESP_LOGE(TAG_RADIO, "Radio init failed"); vTaskDelete(NULL); }

    while (true) {
        SensorReading r;
        xQueueReceive(q_radio, &r, portMAX_DELAY);

        // Pack 4 floats into a 16-byte compact payload
        float payload[4] = { r.temp, r.pressure, r.humidity, r.az };
        radio.send((const uint8_t*)payload, sizeof(payload));

        vTaskDelay(pdMS_TO_TICKS(5000));  // respect EU duty cycle
    }
}

static void monitor_task(void* arg) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        ESP_LOGI(TAG_MONITOR, "Stack watermarks — sensor:%lu sd:%lu radio:%lu",
                 (unsigned long)uxTaskGetStackHighWaterMark(h_sensor),
                 (unsigned long)uxTaskGetStackHighWaterMark(h_sd),
                 (unsigned long)uxTaskGetStackHighWaterMark(h_radio));
        ESP_LOGI(TAG_MONITOR, "Queue depths — sd:%lu/%d radio:%lu/%d",
                 (unsigned long)uxQueueMessagesWaiting(q_sd),    8,
                 (unsigned long)uxQueueMessagesWaiting(q_radio),  4);
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

    s_i2c_mutex = xSemaphoreCreateMutex();
    q_sd        = xQueueCreate(8, sizeof(SensorReading));
    q_radio     = xQueueCreate(4, sizeof(SensorReading));

    xTaskCreate(sensor_task,  "sensor",  4096, NULL, 5, &h_sensor);
    xTaskCreate(sd_task,      "sd",      8192, NULL, 4, &h_sd);
    xTaskCreate(radio_task,   "radio",   4096, NULL, 3, &h_radio);
    xTaskCreate(monitor_task, "monitor", 2048, NULL, 2, NULL);

    vTaskDelete(NULL);
}
