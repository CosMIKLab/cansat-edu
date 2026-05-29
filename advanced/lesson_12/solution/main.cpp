/*
 * Lesson 12 – Solution: WiFi Stack & HTTP Client
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <stdio.h>

static const char* TAG = "Lesson12";

#define WIFI_SSID   "your_ssid"
#define WIFI_PASS   "your_password"
#define SERVER_URL  "http://192.168.1.100:8080/telemetry"

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t s_wifi_event_group;

static void wifi_event_handler(void* arg, esp_event_base_t base,
                               int32_t event_id, void* event_data) {
    if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Disconnected — retrying...");
        esp_wifi_connect();
    } else if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* e = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&e->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_cfg = {};
    strncpy((char*)wifi_cfg.sta.ssid,     WIFI_SSID, sizeof(wifi_cfg.sta.ssid));
    strncpy((char*)wifi_cfg.sta.password, WIFI_PASS, sizeof(wifi_cfg.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_start();
}

static void send_http(float temp, float press, float hum) {
    char url[256];
    snprintf(url, sizeof(url), "%s?temp=%.2f&press=%.2f&hum=%.1f",
             SERVER_URL, temp, press, hum);

    esp_http_client_config_t http_cfg = { .url = url };
    esp_http_client_handle_t client = esp_http_client_init(&http_cfg);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP GET → %d", code);
    } else {
        ESP_LOGE(TAG, "HTTP error: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

extern "C" void app_main(void) {
    s_wifi_event_group = xEventGroupCreate();

    wifi_init();

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(30000));

    if (!(bits & WIFI_CONNECTED_BIT)) {
        ESP_LOGE(TAG, "WiFi connection timed out");
        return;
    }

    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_cfg = {
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .i2c_port          = I2C_NUM_0,
        .sda_io_num        = PIN_SDA,
        .scl_io_num        = PIN_SCL,
        .glitch_ignore_cnt = 7,
        .flags             = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));

    BME280 bme;
    if (!bme.init(bus)) { ESP_LOGE(TAG, "BME280 init failed"); return; }

    while (true) {
        float t, p, h;
        if (bme.read(t, p, h)) {
            send_http(t, p, h);
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
