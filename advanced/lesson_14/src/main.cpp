/*
 * Lesson 14 – Capstone: Full CanSat Mission
 *
 * Learning objectives:
 *  - Load configuration from NVS (Non-Volatile Storage) at boot
 *  - Subscribe tasks to the hardware task watchdog (esp_task_wdt)
 *  - Implement graceful error recovery without halting the mission
 *  - Combine every concept from lessons 1–13 into production-quality firmware
 *
 * System design:
 *
 *   NVS config → boot parameters (tx interval, log interval, radio enable)
 *
 *   sensor_task  →  [q_telem]  →  sd_task     (always on)
 *                              →  radio_task   (conditional on config)
 *
 *   watchdog_task — kicks esp_task_wdt; kills mission if sensors silent > 10 s
 *
 * Your task:
 *  1. Read mission parameters from NVS using the helpers below.
 *  2. Wire up the task graph (queues, mutex, priorities) — same as Lesson 13.
 *  3. Subscribe sensor_task and watchdog_task to the hardware WDT.
 *  4. Implement graceful error handling: if a sensor read fails 3 times in a row,
 *     log the failure but do NOT restart — keep the other subsystems alive.
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

// ─── NVS configuration keys ──────────────────────────────────────────────────
// namespace "mission", keys: "tx_ms", "log_ms", "radio_en"
// Defaults: tx_ms=10000, log_ms=2000, radio_en=1

struct MissionConfig {
    uint32_t tx_interval_ms;   // radio TX period
    uint32_t log_interval_ms;  // sensor sample period
    bool     radio_enabled;
};

static MissionConfig s_cfg;

static void load_config(void) {
    nvs_handle_t h;
    if (nvs_open("mission", NVS_READONLY, &h) != ESP_OK) {
        // No config in NVS → use defaults
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

// ─── Shared state ─────────────────────────────────────────────────────────────

struct TelemetryPacket {
    uint32_t time_ms;
    float temp, pressure, humidity;
    float ax, ay, az;
    float gx, gy, gz;
    uint8_t error_flags;  // bit 0: bme fail, bit 1: imu fail
};

static BME280   bme;
static LSM6DSOX imu;
static Radio    radio;

static i2c_master_bus_handle_t s_bus;
static SemaphoreHandle_t       s_i2c_mutex;

// Declare these so the skeleton compiles — your task is to create them in app_main()
static QueueHandle_t q_sd    = nullptr;
static QueueHandle_t q_radio = nullptr;
static TaskHandle_t  h_sensor = nullptr;
static TaskHandle_t  h_wdt    = nullptr;

// ─── Tasks ───────────────────────────────────────────────────────────────────

static void sensor_task(void* arg) {
    // TODO: Subscribe this task to the hardware WDT:
    //   esp_task_wdt_add(NULL)  — NULL means current task

    bool bme_ok, imu_ok;
    xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);
    bme_ok = bme.init(s_bus);
    imu_ok = imu.init(s_bus);
    xSemaphoreGive(s_i2c_mutex);

    if (!bme_ok) ESP_LOGE(TAG, "BME280 init failed");
    if (!imu_ok) ESP_LOGW(TAG, "LSM6DSOX init failed");

    uint8_t bme_fail = 0, imu_fail = 0;

    while (true) {
        // TODO: Kick the hardware WDT: esp_task_wdt_reset()

        TelemetryPacket pkt = {};
        pkt.time_ms    = (uint32_t)(esp_timer_get_time() / 1000);
        pkt.error_flags = 0;

        xSemaphoreTake(s_i2c_mutex, portMAX_DELAY);

        // BME280 read with consecutive-failure counter
        float t, p, h;
        if (bme_ok && bme.read(t, p, h)) {
            pkt.temp = t; pkt.pressure = p; pkt.humidity = h;
            bme_fail = 0;
        } else {
            // TODO: Increment bme_fail. If bme_fail >= 3, set bit 0 of pkt.error_flags
            //   and log a warning "BME280: 3 consecutive failures".
        }

        // IMU read with consecutive-failure counter
        ImuData d;
        if (imu_ok && imu.read(d)) {
            pkt.ax = d.ax; pkt.ay = d.ay; pkt.az = d.az;
            pkt.gx = d.gx; pkt.gy = d.gy; pkt.gz = d.gz;
            imu_fail = 0;
        } else {
            // TODO: Increment imu_fail. If imu_fail >= 3, set bit 1 of pkt.error_flags.
        }

        xSemaphoreGive(s_i2c_mutex);

        // TODO: Send pkt to q_sd (timeout 0, log warning if full).
        // TODO: If s_cfg.radio_enabled, send pkt to q_radio (timeout 0).

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
        // TODO: Receive from q_sd (block indefinitely).
        //   Convert TelemetryPacket to TelemetryRecord and call Storage::log().
    }
}

static void radio_task(void* arg) {
    if (!radio.init()) { ESP_LOGE(TAG, "Radio init failed"); vTaskDelete(NULL); }

    while (true) {
        TelemetryPacket pkt;
        // TODO: Receive from q_radio (block indefinitely).
        //   Build a compact payload: [time_ms, temp, pressure, humidity, az, error_flags]
        //   and call radio.send().
        vTaskDelay(pdMS_TO_TICKS(s_cfg.tx_interval_ms));
    }
}

static void wdt_task(void* arg) {
    // TODO: Subscribe this task to the hardware WDT.
    // This task acts as a system heartbeat — if sensor_task stops calling
    // esp_task_wdt_reset(), both tasks eventually trigger the WDT reset.
    while (true) {
        // TODO: Kick the WDT: esp_task_wdt_reset()
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ─── Entry point ─────────────────────────────────────────────────────────────

extern "C" void app_main(void) {
    // NVS init (required for WiFi calibration and our config)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    load_config();

    // I2C bus
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

    // TODO: Create q_sd (depth 16) and q_radio (depth 8).

    // Hardware WDT — timeout 30 s; tasks must reset it within 30 s or system reboots
    // TODO: Configure the task WDT: esp_task_wdt_config_t wdt_cfg = {
    //   .timeout_ms = 30000, .idle_core_mask = 0, .trigger_panic = true };
    //   esp_task_wdt_reconfigure(&wdt_cfg);

    // TODO: Create tasks with appropriate stacks and priorities.
    //   sensor_task:  prio 5, stack 4096, handle → h_sensor
    //   sd_task:      prio 4, stack 8192
    //   radio_task:   prio 3, stack 4096  (only if s_cfg.radio_enabled)
    //   wdt_task:     prio 6, stack 2048, handle → h_wdt

    vTaskDelete(NULL);
}
