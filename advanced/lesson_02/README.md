# Lesson 2 — I2C Driver & BME280 Environmental Sensor

## Learning Objectives
- Configure the ESP-IDF I2C master bus (new v5 API)
- Understand how register-level I2C drivers work
- Read temperature, pressure, and humidity from the Bosch BME280

## Key Concepts

### ESP-IDF I2C Master (v5 API)
```c
// 1. Create the bus
i2c_master_bus_config_t bus_cfg = {
    .clk_source        = I2C_CLK_SRC_DEFAULT,
    .i2c_port          = I2C_NUM_0,
    .sda_io_num        = GPIO_NUM_8,
    .scl_io_num        = GPIO_NUM_9,
    .glitch_ignore_cnt = 7,
    .flags = { .enable_internal_pullup = true },
};
i2c_master_bus_handle_t bus;
i2c_new_master_bus(&bus_cfg, &bus);

// 2. Add a device
i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address  = 0x76,  // BME280 default address
    .scl_speed_hz    = 400000,
};
i2c_master_dev_handle_t dev;
i2c_master_bus_add_device(bus, &dev_cfg, &dev);

// 3. Read a register
uint8_t reg = 0xD0;
uint8_t val;
i2c_master_transmit_receive(dev, &reg, 1, &val, 1, 100 /*ms*/);

// 4. Write a register
uint8_t buf[2] = {0xF4, 0x25};
i2c_master_transmit(dev, buf, 2, 100);
```

### BME280 Highlights
- Chip ID register `0xD0` must return `0x60` to confirm identity
- Read 32 factory calibration coefficients from flash (`0x88`–`0xE7`)
- Trigger a forced-mode measurement then poll the status register
- Apply Bosch's integer compensation formulas to convert raw ADC values

## Hardware Wiring
| BME280 Pin | ESP32-S3 Pin |
|------------|-------------|
| VCC        | 3.3 V       |
| GND        | GND         |
| SDA        | GPIO 8      |
| SCL        | GPIO 9      |
| SDO        | GND (→ addr 0x76) |
| CSB        | 3.3 V (I2C mode) |

## Build & Flash
```
pio run -t upload && pio device monitor
```
Expected output every 2 s:
```
I (1234) Lesson2: Temp: 23.45 °C  Pressure: 1013.25 hPa  Humidity: 48.2 %
```
