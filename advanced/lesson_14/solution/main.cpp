/*
 * Lesson 14 – Solution: Capstone — Full CanSat Mission
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "radio.hpp"
#include "storage.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static const char* TAG = "Mission";

struct MissionConfig {
    uint32_t tx_interval_ms;
    uint32_t log_interval_ms;
    bool     radio_enabled;
};

static MissionConfig s_cfg;

static void load_config(void) {
    nvs_handle_t h;
    if (nvs_open("mission", NVS_READONLY, &h) != ESP_OK) {
        s_cfg = { .tx_interval_ms = 10000, .log_interval_ms = 2000, .radio_enabled = true };
        ESP_LOGW(TAG, "NVS not found — using defaults");
        return;
    }
    uint32_t val;
    s_cfg.tx_interval_ms  = (nvs_get_u32(h, "tx_ms",   &val) == ESP_OK) ? val : 10000;
    s_cfg.log_interval_ms = (nvs_get_u32(h, "log_ms",  &val) == ESP_OK) ? val : 2000;
    uint8_t radio_en;
    s_cfg.radio_enabled   = (nvs_get_u8(h,  "radio_en",&radio_en) == ESP_OK) ? radio_en : 1;
    nvs_close(h);
    ESP_LOGI(TAG, "Config: tx=%lums log=%lums radio=%s",
             (unsigned long)s_cfg.tx_interval_ms,
             (unsigned long)s_cfg.log_interval_ms,
             s_cfg.radio_enabled ? "ON" : "OFF");
}

struct TelemetryPacket {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
    float gx, gy, gz;
    uint8_t error_flags;
};

static BME280   bme;
static LSM6DSOX imu;
static Radio    radio;

static i2c_master_bus_handle_t s_bus;
static SemaphoreHandle_t       s_i2c_mutex;
static QueueHandle_t           q_sd;
static QueueHandle_t           q_radio;
static TaskHandle_t            h_sensor;
static TaskHandle_t            h_wdt;

static void sensor_task(void* arg) {
    esp_task_wdt_add(NULL);

    bool bme_ok, imu_ok;
    xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);
    bme_ok = bme.init(s_bus);
    imu_ok = imu.init(s_bus);
    xSemaphoreGive(s_i2c_mutex);

    if (!bme_ok) ESP_LOGE(TAG, "BME280 init failed");
    if (!imu_ok) ESP_LOGW(TAG, "LSM6DSOX init failed");

    uint8_t bme_fail = 0, imu_fail = 0;

    while (true) {
        esp_task_wdt_reset();

        TelemetryPacket pkt = {};
        pkt.time_ms = (uint32_t)(esp_timer_get_time() / 1000);

        xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);

        float t, p, h;
        if (bme_ok && bme.read(t, p, h)) {
            pkt.temp = t; pkt.pressure = p; pkt.humidity = h;
            bme_fail = 0;
        } else {
            if (++bme_fail >= 3) {
                pkt.error_flags |= 0x01;
                ESP_LOGW(TAG, "BME280: 3 consecutive failures");
            }
        }

        ImuData d;
        if (imu_ok && imu.read(d)) {
            pkt.ax = d.ax; pkt.ay = d.ay; pkt.az = d.az;
            pkt.gx = d.gx; pkt.gy = d.gy; pkt.gz = d.gz;
            imu_fail = 0;
        } else {
            if (++imu_fail >= 3) pkt.error_flags |= 0x02;
        }

        xSemaphoreGive(s_i2c_mutex);

        if (xQueueSend(q_sd, &pkt, 0) != pdTRUE)
            ESP_LOGW(TAG, "SD queue full");
        if (s_cfg.radio_enabled && xQueueSend(q_radio, &pkt, 0) != pdTRUE)
            ESP_LOGW(TAG, "Radio queue full");

        ESP_LOGI(TAG, "[%lums] T:%.2f P:%.2f H:%.1f err:0x%02X",
                 (unsigned long)pkt.time_ms,
                 pkt.temp, pkt.pressure, pkt.humidity, pkt.error_flags);

        vTaskDelay(pdMS_TO_TICKS(s_cfg.log_interval_ms));
    }
}

static void sd_task(void* arg) {
    if (!Storage::init()) { ESP_LOGE(TAG, "Storage init failed"); vTaskDelete(NULL); }
    Storage::event("MISSION_START");

    while (true) {
        TelemetryPacket pkt;
        xQueueReceive(q_sd, &pkt, portMAX_DELAY);

        TelemetryRecord rec = {
            .time_ms  = pkt.time_ms,
            .temp     = pkt.temp,
            .pressure = pkt.pressure,
            .humidity = pkt.humidity,
            .ax = pkt.ax, .ay = pkt.ay, .az = pkt.az,
            .gx = pkt.gx, .gy = pkt.gy, .gz = pkt.gz,
        };
        Storage::log(rec);
    }
}

static void radio_task(void* arg) {
    if (!radio.init()) { ESP_LOGE(TAG, "Radio init failed"); vTaskDelete(NULL); }

    while (true) {
        TelemetryPacket pkt;
        xQueueReceive(q_radio, &pkt, portMAX_DELAY);

        struct __attribute__((packed)) Payload {
            uint32_t time_ms;
            float temp, pressure, humidity, az;
            uint8_t err;
        } payload = {
            pkt.time_ms, pkt.temp, pkt.pressure, pkt.humidity, pkt.az, pkt.error_flags
        };
        radio.send((const uint8_t*)&payload, sizeof(payload));

        vTaskDelay(pdMS_TO_TICKS(s_cfg.tx_interval_ms));
    }
}

static void wdt_task(void* arg) {
    esp_task_wdt_add(NULL);
    while (true) {
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    load_config();

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
    q_sd        = xQueueCreate(16, sizeof(TelemetryPacket));
    q_radio     = xQueueCreate(8,  sizeof(TelemetryPacket));

    esp_task_wdt_config_t wdt_cfg = {
        .timeout_ms     = 30000,
        .idle_core_mask = 0,
        .trigger_panic  = true,
    };
    esp_task_wdt_reconfigure(&wdt_cfg);

    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, &h_sensor);
    xTaskCreate(sd_task,     "sd",     8192, NULL, 4, NULL);
    if (s_cfg.radio_enabled) {
        xTaskCreate(radio_task, "radio", 4096, NULL, 3, NULL);
    }
    xTaskCreate(wdt_task, "wdt", 2048, NULL, 6, &h_wdt);

    vTaskDelete(NULL);
}
