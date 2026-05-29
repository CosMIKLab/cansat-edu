/*
 * Lesson 13 – Full Telemetry Pipeline
 *
 * Learning objectives:
 *  - Design a multi-task pipeline that processes data through multiple stages
 *  - Fan-out a queue to two consumers (SD logger and radio transmitter)
 *  - Use a supervisor task to monitor system health via stack watermarks
 *
 * Architecture:
 *
 *   sensor_task  →  [q_sd]    →  sd_task
 *                →  [q_radio] →  radio_task
 *
 *   monitor_task — prints stack watermarks every 10 s
 *
 * All sensor reads are protected by s_i2c_mutex.
 * sensor_task sends copies of SensorReading to both queues.
 *
 * Your task:
 *  Wire together the queues, tasks, and mutex. The sensor_task body and
 *  driver inits are provided — you complete the queue declarations,
 *  task bodies, and the task-creation calls in app_main().
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

// TODO: Declare two QueueHandle_t variables: q_sd and q_radio.
// They must be global so monitor_task can access them.
// Hint: static QueueHandle_t q_sd = nullptr;
static QueueHandle_t q_sd    = nullptr;
static QueueHandle_t q_radio = nullptr;

// Handles for the monitor task to inspect
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

        // TODO: Send r to q_sd with xQueueSend() (timeout 0, log warning if full).
        // TODO: Send r to q_radio with xQueueSend() (timeout 0, log warning if full).

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
        // TODO: Receive from q_sd (block indefinitely).
        //   Convert SensorReading to TelemetryRecord and call Storage::log().

        (void)r;  // remove this line when you implement the TODO above
    }
}

static void radio_task(void* arg) {
    if (!radio.init()) { ESP_LOGE(TAG_RADIO, "Radio init failed"); vTaskDelete(NULL); }

    while (true) {
        SensorReading r;
        // TODO: Receive from q_radio (block indefinitely).
        //   Encode the reading as a compact binary packet (e.g., 4 floats = 16 bytes)
        //   and call radio.send().
        //   Respect the 5-second duty cycle: vTaskDelay(pdMS_TO_TICKS(5000)) after each send.

        (void)r;  // remove this line when you implement the TODO above
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
                 (unsigned long)uxQueueMessagesWaiting(q_sd),   8,
                 (unsigned long)uxQueueMessagesWaiting(q_radio), 4);
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

    // TODO: Create q_sd (depth 8) and q_radio (depth 4).

    // TODO: Create all four tasks. Store the handles in h_sensor, h_sd, h_radio.
    //   sensor_task: priority 5, stack 4096
    //   sd_task:     priority 4, stack 8192 (SD needs more stack)
    //   radio_task:  priority 3, stack 4096
    //   monitor_task:priority 2, stack 2048

    vTaskDelete(NULL);
}
