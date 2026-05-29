# Lesson 10 — UART & LoRa Radio (RN2483)

## Learning Objectives
- Configure a hardware UART port with `uart_driver_install()`
- Implement an AT-command protocol to control the RN2483 LoRa module
- Encode binary payloads as hexadecimal strings for transmission

## Key Concepts

### ESP-IDF UART Driver
```c
uart_config_t cfg = {
    .baud_rate  = 115200,
    .data_bits  = UART_DATA_8_BITS,
    .parity     = UART_PARITY_DISABLE,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
};
uart_param_config(UART_NUM_1, &cfg);
uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
uart_driver_install(UART_NUM_1,
    1024,  // RX buffer size
    0,     // TX buffer (0 = synchronous)
    0, NULL, 0);

// Write
uart_write_bytes(UART_NUM_1, "hello\r\n", 7);

// Read (blocking with timeout)
uint8_t buf[32];
int len = uart_read_bytes(UART_NUM_1, buf, sizeof(buf), pdMS_TO_TICKS(500));
```

### RN2483 AT Commands
```
radio set mod lora\r\n       → set modulation
radio set freq 868100000\r\n → 868.1 MHz
radio set sf sf7\r\n         → spreading factor 7
radio set pa off\r\n         → disable power amplifier boost
radio set pwr 12\r\n         → TX power 12 dBm
radio tx 43414E534154\r\n    → transmit hex payload ("CANSAT")
```

### Hex Encoding
The RN2483 expects payloads as uppercase hex strings:
```c
for (int i = 0; i < len; i++) {
    char hex[3];
    snprintf(hex, 3, "%02X", payload[i]);
    uart_write_bytes(UART_NUM_1, hex, 2);
}
uart_write_bytes(UART_NUM_1, "\r\n", 2);
```

### EU 868 MHz Duty Cycle
The 868 MHz ISM band has a **1% duty cycle** limit. At SF7 a packet takes ~50 ms on-air, meaning **maximum 1 packet per 5 seconds**. Violating this is illegal.

## Hardware
The RN2483 module connects to UART1 (TX=GPIO17, RX=GPIO18). No physical switch is needed on ESP32-S3 — UART0 is reserved for the USB-JTAG debug console.

## Build & Flash
```
pio run -t upload && pio device monitor
```
Use a LoRa gateway or a second RN2483 as receiver to verify packets are transmitted.
