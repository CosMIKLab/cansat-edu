/*
 * Lesson 2 – I2C Driver & BME280 Environmental Sensor
 *
 * Learning objectives:
 *  - Configure the ESP-IDF I2C master bus with i2c_new_master_bus()
 *  - Add a device to the bus with i2c_master_bus_add_device()
 *  - Use a register-level driver (BME280) to read real sensor data
 *
 * Your task:
 *  Complete the TODOs in app_main() to initialise the I2C bus, initialise
 *  the BME280 sensor, and print temperature, pressure, and humidity in a loop.
 */

#include "config.h"
#include "bme280.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

static const char* TAG = "Lesson2";

static BME280 bme;

extern "C" void app_main(void) {

    // TODO: Declare an i2c_master_bus_handle_t variable named bus_handle.

    // TODO: Fill in an i2c_master_bus_config_t struct:
    //   .clk_source        = I2C_CLK_SRC_DEFAULT
    //   .i2c_port          = I2C_NUM_0
    //   .sda_io_num        = PIN_SDA
    //   .scl_io_num        = PIN_SCL
    //   .glitch_ignore_cnt = 7
    //   .flags.enable_internal_pullup = true
    // Then call i2c_new_master_bus(&cfg, &bus_handle) and check the return value.

    // TODO: Call bme.init(bus_handle).
    //   If it returns false, log an error and return.

    while (true) {
        float temp, press, hum;

        // TODO: Call bme.read(temp, press, hum).
        //   If it returns true, log the three values with ESP_LOGI.
        //   If it returns false, log a warning "BME280 read failed".

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
