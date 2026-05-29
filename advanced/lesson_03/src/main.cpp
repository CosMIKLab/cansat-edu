/*
 * Lesson 3 – I2C Multi-Device & LSM6DSOX IMU
 *
 * Learning objectives:
 *  - Share one I2C bus between multiple devices at different addresses
 *  - Burst-read 12 bytes from the LSM6DSOX output registers
 *  - Apply sensitivity scaling to convert raw ADC values to physical units
 *
 * Your task:
 *  Create the I2C bus, initialise BOTH the BME280 and LSM6DSOX on the
 *  same bus, then read and print all sensor data every 2 seconds.
 *
 *  NOTE: Both sensors share bus_handle — create it once and pass it to
 *        both init() calls.
 */

#include "config.h"
#include "bme280.hpp"
#include "lsm6dsox.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

static const char* TAG = "Lesson3";

static BME280   bme;
static LSM6DSOX imu;

extern "C" void app_main(void) {

    // TODO: Create the I2C master bus (same as Lesson 2).

    // TODO: Call bme.init(bus_handle).
    //   Log an error and return on failure.

    // TODO: Call imu.init(bus_handle).
    //   Log a warning (non-fatal) on failure; continue even if IMU fails.

    while (true) {
        float temp, press, hum;
        if (bme.read(temp, press, hum)) {
            ESP_LOGI(TAG, "BME280 — T:%.2f°C  P:%.2fhPa  H:%.1f%%",
                     temp, press, hum);
        }

        ImuData d;
        // TODO: Call imu.read(d). On success log ax/ay/az and gx/gy/gz.
        // Format: "IMU — ax:%.3f ay:%.3f az:%.3f  gx:%.2f gy:%.2f gz:%.2f"

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
