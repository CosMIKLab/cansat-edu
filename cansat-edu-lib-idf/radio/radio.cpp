#include "radio.hpp"
#include "config.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char* TAG = "Radio";

// SX126x command opcodes (E22-900M22S module, SX1262 core)
#define CMD_SET_STANDBY           0x80
#define CMD_SET_PACKET_TYPE       0x8A
#define CMD_SET_RF_FREQUENCY      0x86
#define CMD_SET_BUFFER_BASE_ADDR  0x8F
#define CMD_WRITE_BUFFER          0x0E
#define CMD_READ_BUFFER           0x1E
#define CMD_SET_MODULATION_PARAMS 0x8B
#define CMD_SET_PACKET_PARAMS     0x8C
#define CMD_SET_TX_PARAMS         0x8E
#define CMD_SET_PA_CONFIG         0x95
#define CMD_SET_DIO_IRQ_PARAMS    0x08
#define CMD_SET_TX                0x83
#define CMD_SET_RX                0x82
#define CMD_GET_IRQ_STATUS        0x12
#define CMD_CLEAR_IRQ_STATUS      0x02
#define CMD_GET_RX_BUFFER_STATUS  0x13
#define CMD_GET_STATUS            0xC0

#define PACKET_TYPE_LORA 0x01

#define IRQ_TX_DONE 0x0001
#define IRQ_RX_DONE 0x0002
#define IRQ_TIMEOUT 0x0200
#define IRQ_ALL     0xFFFF

void Radio::waitWhileBusy(uint16_t timeoutMs) {
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);
    while (gpio_get_level(PIN_RADIO_BUSY)) {
        if (xTaskGetTickCount() > deadline) return;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void Radio::reset() {
    gpio_set_level(PIN_RADIO_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_RADIO_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
    waitWhileBusy(500);
}

void Radio::writeCommand(uint8_t opcode, const uint8_t* params, uint8_t len) {
    waitWhileBusy(100);
    uint8_t tx[1 + 32] = {};
    tx[0] = opcode;
    memcpy(tx + 1, params, len);

    spi_transaction_t t = {};
    t.length    = (1 + len) * 8;
    t.tx_buffer = tx;
    spi_device_transmit(_dev, &t);
}

void Radio::readCommand(uint8_t opcode, uint8_t* result, uint8_t len) {
    waitWhileBusy(100);
    uint8_t tx[2 + 32] = {};
    uint8_t rx[2 + 32] = {};
    tx[0] = opcode;

    spi_transaction_t t = {};
    t.length    = (2 + len) * 8;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    spi_device_transmit(_dev, &t);

    memcpy(result, rx + 2, len);
}

void Radio::writeBuffer(uint8_t offset, const uint8_t* data, uint8_t len) {
    waitWhileBusy(100);
    uint8_t tx[2 + 255] = {};
    tx[0] = CMD_WRITE_BUFFER;
    tx[1] = offset;
    memcpy(tx + 2, data, len);

    spi_transaction_t t = {};
    t.length    = (2 + len) * 8;
    t.tx_buffer = tx;
    spi_device_transmit(_dev, &t);
}

void Radio::readBuffer(uint8_t offset, uint8_t* data, uint8_t len) {
    waitWhileBusy(100);
    uint8_t tx[3 + 255] = {};
    uint8_t rx[3 + 255] = {};
    tx[0] = CMD_READ_BUFFER;
    tx[1] = offset;

    spi_transaction_t t = {};
    t.length    = (3 + len) * 8;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    spi_device_transmit(_dev, &t);

    memcpy(data, rx + 3, len);
}

uint8_t Radio::readStatusByte() {
    waitWhileBusy(100);
    uint8_t tx[2] = { CMD_GET_STATUS, 0x00 };
    uint8_t rx[2] = {};

    spi_transaction_t t = {};
    t.length    = 16;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    spi_device_transmit(_dev, &t);

    return rx[1];
}

uint16_t Radio::getIrqStatus() {
    uint8_t raw[2] = {};
    readCommand(CMD_GET_IRQ_STATUS, raw, sizeof(raw));
    return ((uint16_t)raw[0] << 8) | raw[1];
}

void Radio::clearIrqStatus(uint16_t mask) {
    uint8_t params[2] = { (uint8_t)(mask >> 8), (uint8_t)(mask & 0xFF) };
    writeCommand(CMD_CLEAR_IRQ_STATUS, params, sizeof(params));
}

static uint8_t bandwidthToReg(uint16_t khz) {
    switch (khz) {
        case 125: return 0x04;
        case 250: return 0x05;
        case 500: return 0x06;
        default:  return 0x04;
    }
}

static uint8_t codingRateToReg(uint8_t denominator) {
    switch (denominator) {
        case 5: return 0x01;  // 4/5
        case 6: return 0x02;  // 4/6
        case 7: return 0x03;  // 4/7
        case 8: return 0x04;  // 4/8
        default: return 0x01;
    }
}

void Radio::configureModem() {
    uint8_t packetType = PACKET_TYPE_LORA;
    writeCommand(CMD_SET_PACKET_TYPE, &packetType, 1);

    // RF frequency register = freq_hz * 2^25 / 32 MHz (SX126x XTAL reference)
    uint32_t freqReg = (uint32_t)(((uint64_t)RADIO_FREQ_HZ << 25) / 32000000ULL);
    uint8_t freqParams[4] = {
        (uint8_t)(freqReg >> 24), (uint8_t)(freqReg >> 16),
        (uint8_t)(freqReg >> 8),  (uint8_t)(freqReg)
    };
    writeCommand(CMD_SET_RF_FREQUENCY, freqParams, sizeof(freqParams));

    uint8_t modParams[4] = { RADIO_SF, bandwidthToReg(RADIO_BW_KHZ), codingRateToReg(RADIO_CR), 0x00 };
    writeCommand(CMD_SET_MODULATION_PARAMS, modParams, sizeof(modParams));

    // Preamble 8 symbols, explicit header, payload length patched per send() call, CRC on, standard IQ
    uint8_t pktParams[9] = { 0x00, 0x08, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00 };
    writeCommand(CMD_SET_PACKET_PARAMS, pktParams, sizeof(pktParams));

    // High-power PA config for the SX1262 core, up to +22 dBm
    uint8_t paParams[4] = { 0x04, 0x07, 0x00, 0x01 };
    writeCommand(CMD_SET_PA_CONFIG, paParams, sizeof(paParams));

    uint8_t txParams[2] = { (uint8_t)RADIO_POWER_DBM, 0x02 };  // ramp time 40us
    writeCommand(CMD_SET_TX_PARAMS, txParams, sizeof(txParams));

    uint8_t bufParams[2] = { 0x00, 0x00 };  // tx/rx base address
    writeCommand(CMD_SET_BUFFER_BASE_ADDR, bufParams, sizeof(bufParams));

    uint8_t irqParams[8] = {
        (uint8_t)(IRQ_ALL >> 8), (uint8_t)(IRQ_ALL & 0xFF),
        (uint8_t)(IRQ_ALL >> 8), (uint8_t)(IRQ_ALL & 0xFF),
        0x00, 0x00, 0x00, 0x00
    };
    writeCommand(CMD_SET_DIO_IRQ_PARAMS, irqParams, sizeof(irqParams));
    clearIrqStatus(IRQ_ALL);
}

bool Radio::init(spi_host_device_t host) {
    gpio_config_t out_cfg = {};
    out_cfg.pin_bit_mask = (1ULL << PIN_RADIO_RESET) | (1ULL << PIN_RADIO_RXEN);
    out_cfg.mode         = GPIO_MODE_OUTPUT;
    gpio_config(&out_cfg);

    gpio_config_t in_cfg = {};
    in_cfg.pin_bit_mask = (1ULL << PIN_RADIO_BUSY) | (1ULL << PIN_RADIO_DIO1);
    in_cfg.mode         = GPIO_MODE_INPUT;
    gpio_config(&in_cfg);

    gpio_set_level(PIN_RADIO_RXEN, 0);

    spi_device_interface_config_t dev_cfg = {};
    dev_cfg.mode           = 0;
    dev_cfg.clock_speed_hz = 1000000;
    dev_cfg.spics_io_num   = PIN_RADIO_CS;
    dev_cfg.queue_size     = 1;
    if (spi_bus_add_device(host, &dev_cfg, &_dev) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add radio to SPI bus");
        return false;
    }

    reset();

    uint8_t standbyParam = 0x00;  // STDBY_RC
    writeCommand(CMD_SET_STANDBY, &standbyParam, 1);

    uint8_t status = readStatusByte();
    _ready = (status != 0x00 && status != 0xFF);
    if (!_ready) {
        ESP_LOGE(TAG, "Radio not responding (status=0x%02X)", status);
        return false;
    }

    configureModem();
    ESP_LOGI(TAG, "Initialized OK");
    return true;
}

bool Radio::present() const {
    return _ready;
}

bool Radio::send(const uint8_t* payload, uint8_t len) {
    if (!_ready) return false;

    gpio_set_level(PIN_RADIO_RXEN, 0);  // TX path — RXEN gates the antenna switch

    writeBuffer(0x00, payload, len);

    uint8_t pktParams[9] = { 0x00, 0x08, 0x00, len, 0x01, 0x00, 0x00, 0x00, 0x00 };
    writeCommand(CMD_SET_PACKET_PARAMS, pktParams, sizeof(pktParams));

    clearIrqStatus(IRQ_ALL);

    uint8_t timeoutParams[3] = { 0x00, 0x0F, 0xA0 };  // ~1s, in 15.625us steps
    writeCommand(CMD_SET_TX, timeoutParams, sizeof(timeoutParams));

    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(2000);
    while (xTaskGetTickCount() < deadline) {
        uint16_t irq = getIrqStatus();
        if (irq & IRQ_TX_DONE) {
            clearIrqStatus(IRQ_ALL);
            return true;
        }
        if (irq & IRQ_TIMEOUT) {
            clearIrqStatus(IRQ_ALL);
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return false;
}

int Radio::receive(uint8_t* buffer, uint8_t maxLen, uint32_t timeoutMs) {
    if (!_ready) return -1;

    gpio_set_level(PIN_RADIO_RXEN, 1);  // RX path — RXEN gates the antenna switch

    clearIrqStatus(IRQ_ALL);

    uint32_t timeoutUnits = timeoutMs * 64;  // 15.625us steps
    uint8_t timeoutParams[3] = {
        (uint8_t)(timeoutUnits >> 16), (uint8_t)(timeoutUnits >> 8), (uint8_t)(timeoutUnits)
    };
    writeCommand(CMD_SET_RX, timeoutParams, sizeof(timeoutParams));

    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs + 200);
    while (xTaskGetTickCount() < deadline) {
        uint16_t irq = getIrqStatus();
        if (irq & IRQ_RX_DONE) {
            uint8_t status[2] = {};
            readCommand(CMD_GET_RX_BUFFER_STATUS, status, sizeof(status));
            uint8_t payloadLen = status[0];
            uint8_t startAddr  = status[1];
            uint8_t toRead = (payloadLen < maxLen) ? payloadLen : maxLen;
            readBuffer(startAddr, buffer, toRead);
            clearIrqStatus(IRQ_ALL);
            gpio_set_level(PIN_RADIO_RXEN, 0);
            return toRead;
        }
        if (irq & IRQ_TIMEOUT) {
            clearIrqStatus(IRQ_ALL);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    gpio_set_level(PIN_RADIO_RXEN, 0);
    return -1;
}
