#ifndef SENSORS_HPP
#define SENSORS_HPP

#include <Arduino.h>

// Define the struct type first
typedef struct {
    uint8_t (*init)(void);
    uint8_t (*read_temp)(float* temp, float* press, float* hum);
} bme_t;

// Declare the external instance
extern bme_t bme;

// --- LSM6DSOX ---
typedef struct {
    float ax, ay, az; // Accelerometer values in g
    float gx, gy, gz; // Gyroscope values in dps
} imu_data_t;

typedef struct {
    uint8_t (*init)(void);
    uint8_t (*read)(imu_data_t* data);
} imu_t;

extern imu_t imu;

#endif /* SENSORS_HPP */
