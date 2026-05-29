/*
 * Lesson 10 – UART & LoRa Radio (RN2483)
 *
 * Learning objectives:
 *  - Configure UART1 with uart_driver_install() and uart_param_config()
 *  - Send AT-style commands to the RN2483 LoRa module
 *  - Encode a binary payload as a hex string for "radio tx" transmission
 *
 * Protocol (RN2483 LoRa):
 *   - Configure: "radio set mod lora\r\n", "radio set freq 868100000\r\n", etc.
 *   - Transmit:  "radio tx AABBCCDD\r\n"  (hex-encoded payload)
 *   - EU duty cycle: max ~1 packet / 5 s at SF7
 *
 * Your task:
 *  Complete the TODOs so the radio initialises and sends a 6-byte test
 *  packet (the word "CANSAT" as ASCII bytes) every 10 seconds.
 */

#include "config.h"
#include "radio.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Lesson10";

static Radio radio;

extern "C" void app_main(void) {

    // TODO: Call radio.init().
    //   If it returns false, log an error and return.
    //   radio.init() configures UART1 and sends the LoRa settings.

    const uint8_t payload[] = {'C', 'A', 'N', 'S', 'A', 'T'};
    int packet_count = 0;

    while (true) {
        ESP_LOGI(TAG, "Sending packet #%d", packet_count++);

        // TODO: Call radio.send(payload, sizeof(payload)).
        //   If it returns false, log a warning.

        // EU 868 MHz duty cycle: wait at least 5 seconds between packets at SF7.
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
