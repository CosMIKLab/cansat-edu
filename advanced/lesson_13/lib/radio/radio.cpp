#include "radio.hpp"
#include "config.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char* TAG = "Radio";

const char* Radio::_settings[Radio::SETTINGS_LEN] = {
    "mod lora",
    "freq 868100000",
    "sf sf7",
    "pa off",
    "pwr 12"
};

void Radio::_sendCmd(const char* cmd) {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "radio set %s\r\n", cmd);
    uart_write_bytes(UART_RADIO, buf, len);
}

bool Radio::init() {
    uart_config_t uart_cfg = {
        .baud_rate           = 115200,
        .data_bits           = UART_DATA_8_BITS,
        .parity              = UART_PARITY_DISABLE,
        .stop_bits           = UART_STOP_BITS_1,
        .flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
    };
    uart_param_config(UART_RADIO, &uart_cfg);
    uart_set_pin(UART_RADIO, PIN_RADIO_TX, PIN_RADIO_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_RADIO, 1024, 0, 0, NULL, 0);

    for (uint8_t i = 0; i < SETTINGS_LEN; i++) {
        _sendCmd(_settings[i]);
    }
    ESP_LOGI(TAG, "LoRa configured");
    return true;
}

bool Radio::send(const uint8_t* payload, uint8_t len) {
    char header[] = "radio tx ";
    uart_write_bytes(UART_RADIO, header, strlen(header));

    char hex[3];
    for (uint8_t i = 0; i < len; i++) {
        snprintf(hex, sizeof(hex), "%02X", payload[i]);
        uart_write_bytes(UART_RADIO, hex, 2);
    }
    uart_write_bytes(UART_RADIO, "\r\n", 2);
    ESP_LOGI(TAG, "Sent %d bytes", len);
    return true;
}
