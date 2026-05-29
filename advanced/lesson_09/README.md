# Lesson 9 — SD Card & FAT Filesystem

## Learning Objectives
- Mount a FAT-formatted SD card over SPI using `esp_vfs_fat_sdspi_mount()`
- Use standard POSIX file I/O (`fopen`, `fprintf`, `fflush`, `fclose`) via VFS
- Understand session-based data organisation for flight logging

## Key Concepts

### SPI SD Card Mounting
```c
sdmmc_host_t host = SDSPI_HOST_DEFAULT();  // uses SPI2_HOST by default

spi_bus_config_t bus = {
    .mosi_io_num = GPIO_NUM_11,
    .miso_io_num = GPIO_NUM_13,
    .sclk_io_num = GPIO_NUM_12,
    .quadwp_io_num = -1, .quadhd_io_num = -1,
};
spi_bus_initialize(host.slot, &bus, SDSPI_DEFAULT_DMA);

sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
slot.gpio_cs = GPIO_NUM_10;
slot.host_id = host.slot;

esp_vfs_fat_sdmmc_mount_config_t mnt = {
    .format_if_mount_failed = false,
    .max_files = 5,
    .allocation_unit_size = 16 * 1024,
};

sdmmc_card_t *card;
esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot, &mnt, &card);
```

### POSIX File I/O via VFS
After mounting, files appear under `/sdcard/`. Use standard C file functions:
```c
FILE* f = fopen("/sdcard/S001/telem.csv", "w");
fprintf(f, "time_ms,temp_c\n");
fprintf(f, "%lu,%.2f\n", millis, temp);
fflush(f);   // force to SD card
fclose(f);
```

### Session Management
Each power-on creates a new directory `S001`, `S002`, …, `S999`:
- **telem.csv** — comma-separated sensor data
- **events.txt** — timestamped log messages

This pattern ensures data from each flight is isolated and never overwritten.

## Hardware Wiring
| Signal | ESP32-S3 Pin |
|--------|-------------|
| MOSI   | GPIO 11     |
| MISO   | GPIO 13     |
| CLK    | GPIO 12     |
| CS     | GPIO 10     |
| VCC    | 3.3 V       |
| GND    | GND         |

**SD card must be FAT32 formatted.**

## Build & Flash
```
pio run -t upload && pio device monitor
```
After running, remove the SD card and inspect `S001/telem.csv` and `events.txt` on a PC.
