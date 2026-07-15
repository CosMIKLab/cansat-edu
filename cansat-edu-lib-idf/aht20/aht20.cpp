#include "aht20.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "AHT20";

bool AHT20::init(i2c_master_bus_handle_t bus) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = AHT20_ADDR,
        .scl_speed_hz    = 400000,
    };
    if (i2c_master_bus_add_device(bus, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device to I2C bus");
        return false;
    }

    uint8_t init_cmd[] = {0xBE, 0x08, 0x00};
    if (i2c_master_transmit(_dev, init_cmd, sizeof(init_cmd), 100) != ESP_OK) {
        ESP_LOGE(TAG, "Init command failed");
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_LOGI(TAG, "Initialized OK");
    return true;
}

bool AHT20::read(float& temp, float& hum) {
    uint8_t measure_cmd[] = {0xAC, 0x33, 0x00};
    if (i2c_master_transmit(_dev, measure_cmd, sizeof(measure_cmd), 100) != ESP_OK) return false;
    vTaskDelay(pdMS_TO_TICKS(90));

    uint8_t raw[6];
    if (i2c_master_receive(_dev, raw, sizeof(raw), 100) != ESP_OK) return false;
    if (raw[0] & 0x80) return false;  // still busy

    uint32_t humidityRaw    = ((uint32_t)raw[1] << 12) | ((uint32_t)raw[2] << 4) | (raw[3] >> 4);
    uint32_t temperatureRaw = (((uint32_t)raw[3] & 0x0F) << 16) | ((uint32_t)raw[4] << 8) | raw[5];

    hum  = (float)humidityRaw * 100.0f / 1048576.0f;
    temp = (float)temperatureRaw * 200.0f / 1048576.0f - 50.0f;
    return true;
}
