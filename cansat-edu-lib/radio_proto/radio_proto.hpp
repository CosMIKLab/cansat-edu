#ifndef RADIO_PROTO_HPP
#define RADIO_PROTO_HPP

#include <stdint.h>

// Shared SX126x (E22-900M22S) low-level command/opcode layer used by both the
// full Radio class (../radio) and the simplified RadioEasy facade (../radio_easy).
namespace radio_proto {

// SX126x command opcodes
constexpr uint8_t CMD_SET_STANDBY           = 0x80;
constexpr uint8_t CMD_SET_PACKET_TYPE       = 0x8A;
constexpr uint8_t CMD_SET_RF_FREQUENCY      = 0x86;
constexpr uint8_t CMD_SET_BUFFER_BASE_ADDR  = 0x8F;
constexpr uint8_t CMD_WRITE_BUFFER          = 0x0E;
constexpr uint8_t CMD_READ_BUFFER           = 0x1E;
constexpr uint8_t CMD_SET_MODULATION_PARAMS = 0x8B;
constexpr uint8_t CMD_SET_PACKET_PARAMS     = 0x8C;
constexpr uint8_t CMD_SET_TX_PARAMS         = 0x8E;
constexpr uint8_t CMD_SET_PA_CONFIG         = 0x95;
constexpr uint8_t CMD_SET_DIO_IRQ_PARAMS    = 0x08;
constexpr uint8_t CMD_SET_TX                = 0x83;
constexpr uint8_t CMD_SET_RX                = 0x82;
constexpr uint8_t CMD_GET_IRQ_STATUS        = 0x12;
constexpr uint8_t CMD_CLEAR_IRQ_STATUS      = 0x02;
constexpr uint8_t CMD_GET_RX_BUFFER_STATUS  = 0x13;
constexpr uint8_t CMD_GET_STATUS            = 0xC0;

constexpr uint8_t PACKET_TYPE_LORA = 0x01;

constexpr uint16_t IRQ_TX_DONE = 0x0001;
constexpr uint16_t IRQ_RX_DONE = 0x0002;
constexpr uint16_t IRQ_TIMEOUT = 0x0200;
constexpr uint16_t IRQ_ALL     = 0xFFFF;

void select();
void deselect();
void waitWhileBusy(uint16_t timeoutMs);
void reset();
void writeCommand(uint8_t opcode, const uint8_t* params, uint8_t len);
void readCommand(uint8_t opcode, uint8_t* result, uint8_t len);
void writeBuffer(uint8_t offset, const uint8_t* data, uint8_t len);
void readBuffer(uint8_t offset, uint8_t* data, uint8_t len);
uint8_t  readStatusByte();
uint16_t getIrqStatus();
void     clearIrqStatus(uint16_t mask);

} // namespace radio_proto

#endif /* RADIO_PROTO_HPP */
