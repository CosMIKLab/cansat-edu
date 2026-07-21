#include "radio_easy.hpp"
#include "radio_proto.hpp"
#include <config.hpp>
#include <Arduino.h>

using namespace radio_proto;

// E22-900M22S (SX1262) LoRa radio driver — simplified single-shot send() facade.
// See ../radio for the fuller Radio class (adds receive()); both share the
// low-level SX126x command layer in ../radio_proto.

static bool _ready = false;

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

void RadioEasy::begin() {
    pinMode(PIN_RADIO_BUSY, INPUT);
    pinMode(PIN_RADIO_DIO1, INPUT);
    pinMode(PIN_RADIO_RXEN, OUTPUT);
    digitalWrite(PIN_RADIO_RXEN, LOW);
    pinMode(PIN_RADIO_CS, OUTPUT);
    deselect();

    reset();

    uint8_t standbyParam = 0x00;  // STDBY_RC
    writeCommand(CMD_SET_STANDBY, &standbyParam, 1);

    uint8_t status = readStatusByte();
    _ready = (status != 0x00 && status != 0xFF);
    if (!_ready) {
        Serial.println("Radio: not found");
        return;
    }

    uint8_t packetType = PACKET_TYPE_LORA;
    writeCommand(CMD_SET_PACKET_TYPE, &packetType, 1);

    uint32_t freqReg = (uint32_t)(((uint64_t)RADIO_FREQ_HZ << 25) / 32000000ULL);
    uint8_t freqParams[4] = {
        (uint8_t)(freqReg >> 24), (uint8_t)(freqReg >> 16),
        (uint8_t)(freqReg >> 8),  (uint8_t)(freqReg)
    };
    writeCommand(CMD_SET_RF_FREQUENCY, freqParams, sizeof(freqParams));

    uint8_t modParams[4] = { RADIO_SF, bandwidthToReg(RADIO_BW_KHZ), codingRateToReg(RADIO_CR), 0x00 };
    writeCommand(CMD_SET_MODULATION_PARAMS, modParams, sizeof(modParams));

    uint8_t pktParams[9] = { 0x00, 0x08, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00 };
    writeCommand(CMD_SET_PACKET_PARAMS, pktParams, sizeof(pktParams));

    uint8_t paParams[4] = { 0x04, 0x07, 0x00, 0x01 };  // high-power PA config, up to +22 dBm
    writeCommand(CMD_SET_PA_CONFIG, paParams, sizeof(paParams));

    uint8_t txParams[2] = { (uint8_t)RADIO_POWER_DBM, 0x02 };
    writeCommand(CMD_SET_TX_PARAMS, txParams, sizeof(txParams));

    uint8_t bufParams[2] = { 0x00, 0x00 };
    writeCommand(CMD_SET_BUFFER_BASE_ADDR, bufParams, sizeof(bufParams));

    uint8_t irqParams[8] = {
        (uint8_t)(IRQ_ALL >> 8), (uint8_t)(IRQ_ALL & 0xFF),
        (uint8_t)(IRQ_ALL >> 8), (uint8_t)(IRQ_ALL & 0xFF),
        0x00, 0x00, 0x00, 0x00
    };
    writeCommand(CMD_SET_DIO_IRQ_PARAMS, irqParams, sizeof(irqParams));
    clearIrqStatus(IRQ_ALL);

    Serial.println("Radio: LoRa ready");
}

void RadioEasy::send(String msg) {
    if (!_ready) return;

    uint8_t len = (uint8_t)min((size_t)msg.length(), (size_t)255);
    digitalWrite(PIN_RADIO_RXEN, LOW);  // TX path

    writeBuffer(0x00, (const uint8_t*)msg.c_str(), len);

    uint8_t pktParams[9] = { 0x00, 0x08, 0x00, len, 0x01, 0x00, 0x00, 0x00, 0x00 };
    writeCommand(CMD_SET_PACKET_PARAMS, pktParams, sizeof(pktParams));

    clearIrqStatus(IRQ_ALL);

    uint8_t timeoutParams[3] = { 0x00, 0x0F, 0xA0 };  // ~1s, in 15.625us steps
    writeCommand(CMD_SET_TX, timeoutParams, sizeof(timeoutParams));

    uint32_t deadline = millis() + 2000;
    while (millis() < deadline) {
        uint16_t irq = getIrqStatus();
        if (irq & (IRQ_TX_DONE | IRQ_TIMEOUT)) {
            clearIrqStatus(IRQ_ALL);
            return;
        }
        delay(2);
    }
}

void RadioEasy::send(float a, float b, float c) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f,%.2f,%.2f", a, b, c);
    send(String(buf));
}

RadioEasy radio;
