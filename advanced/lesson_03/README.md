# Lesson 3 — I2C Multi-Device & LSM6DSOX IMU

## Learning Objectives
- Share one I2C bus handle between multiple sensor drivers
- Understand burst reads: reading multiple output registers in one transaction
- Apply sensitivity constants to convert raw 16-bit values to physical units (g, dps)

## Key Concepts

### Sharing One Bus, Multiple Devices
The I2C bus is created once. Each driver adds itself as a device using its own 7-bit address:
```c
// Bus created once in app_main()
i2c_master_bus_handle_t bus;
i2c_new_master_bus(&bus_cfg, &bus);

// Each driver calls this internally during init():
i2c_device_config_t dev_cfg = { .device_address = 0x6A, .scl_speed_hz = 400000 };
i2c_master_bus_add_device(bus, &dev_cfg, &dev_handle);
```

### LSM6DSOX Burst Read
Reading all 6 axes takes one I2C transaction (12 bytes from `0x22`):
```
Byte 0-1:  OUTX_L_G / OUTX_H_G  → gx
Byte 2-3:  OUTY_L_G / OUTY_H_G  → gy
Byte 4-5:  OUTZ_L_G / OUTZ_H_G  → gz
Byte 6-7:  OUTX_L_A / OUTX_H_A  → ax
Byte 8-9:  OUTY_L_A / OUTY_H_A  → ay
Byte 10-11:OUTZ_L_A / OUTZ_H_A  → az
```

### Sensitivity Scaling
```c
// ±2 g at 104 Hz → 0.061 mg/LSB = 0.000061 g/LSB
data.ax = raw_ax * (0.061f / 1000.0f);

// ±250 dps at 104 Hz → 8.75 mdps/LSB = 0.00875 dps/LSB
data.gx = raw_gx * (8.75f / 1000.0f);
```

## Hardware Wiring
| LSM6DSOX Pin | ESP32-S3 Pin |
|--------------|-------------|
| VDD          | 3.3 V       |
| GND          | GND         |
| SDA          | GPIO 8      |
| SCL          | GPIO 9      |
| SA0          | GND (→ addr 0x6A) |
| CS           | 3.3 V (I2C mode)  |

Both BME280 and LSM6DSOX connect to the **same SDA/SCL lines**.

## Build & Flash
```
pio run -t upload && pio device monitor
```
Expected output:
```
I (1234) Lesson3: BME280 — T:23.45°C  P:1013.25hPa  H:48.2%
I (1235) Lesson3: IMU — ax:0.001 ay:-0.003 az:0.998  gx:0.12 gy:-0.05 gz:0.03
```
