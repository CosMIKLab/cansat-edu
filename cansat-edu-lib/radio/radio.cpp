#include "radio.hpp"
#include "radio_proto.hpp"
#include <config.hpp>
#include <Arduino.h>

using namespace radio_proto;

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

bool Radio::init() {
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
    if (!_ready) return false;

    configureModem();
    return true;
}

bool Radio::present() const {
    return _ready;
}

bool Radio::send(const uint8_t* payload, uint8_t len) {
    if (!_ready) return false;

    digitalWrite(PIN_RADIO_RXEN, LOW);  // TX path — RXEN gates the antenna switch

    writeBuffer(0x00, payload, len);

    uint8_t pktParams[9] = { 0x00, 0x08, 0x00, len, 0x01, 0x00, 0x00, 0x00, 0x00 };
    writeCommand(CMD_SET_PACKET_PARAMS, pktParams, sizeof(pktParams));

    clearIrqStatus(IRQ_ALL);

    uint8_t timeoutParams[3] = { 0x00, 0x0F, 0xA0 };  // ~1s, in 15.625us steps
    writeCommand(CMD_SET_TX, timeoutParams, sizeof(timeoutParams));

    uint32_t deadline = millis() + 2000;
    while (millis() < deadline) {
        uint16_t irq = getIrqStatus();
        if (irq & IRQ_TX_DONE) {
            clearIrqStatus(IRQ_ALL);
            return true;
        }
        if (irq & IRQ_TIMEOUT) {
            clearIrqStatus(IRQ_ALL);
            return false;
        }
        delay(2);
    }
    return false;
}

int Radio::receive(uint8_t* buffer, uint8_t maxLen, uint32_t timeoutMs) {
    if (!_ready) return -1;

    digitalWrite(PIN_RADIO_RXEN, HIGH);  // RX path — RXEN gates the antenna switch

    clearIrqStatus(IRQ_ALL);

    uint32_t timeoutUnits = timeoutMs * 64;  // 15.625us steps
    uint8_t timeoutParams[3] = {
        (uint8_t)(timeoutUnits >> 16), (uint8_t)(timeoutUnits >> 8), (uint8_t)(timeoutUnits)
    };
    writeCommand(CMD_SET_RX, timeoutParams, sizeof(timeoutParams));

    uint32_t deadline = millis() + timeoutMs + 200;
    while (millis() < deadline) {
        uint16_t irq = getIrqStatus();
        if (irq & IRQ_RX_DONE) {
            uint8_t status[2] = {};
            readCommand(CMD_GET_RX_BUFFER_STATUS, status, sizeof(status));
            uint8_t payloadLen = status[0];
            uint8_t startAddr  = status[1];
            uint8_t toRead = (payloadLen < maxLen) ? payloadLen : maxLen;
            readBuffer(startAddr, buffer, toRead);
            clearIrqStatus(IRQ_ALL);
            digitalWrite(PIN_RADIO_RXEN, LOW);
            return toRead;
        }
        if (irq & IRQ_TIMEOUT) {
            clearIrqStatus(IRQ_ALL);
            break;
        }
        delay(2);
    }
    digitalWrite(PIN_RADIO_RXEN, LOW);
    return -1;
}

Radio radio;
