#include "sensors.hpp"
#include <Arduino.h> // Required for Serial
#include <Wire.h>

// BME280 default I2C address
#define BME280_ADDR 0x76

// LSM6DSOX default I2C address
#define LSM_ADDR 0x6A

// LSM6DSOX Registers
#define LSM_REG_WHO_AM_I   0x0F
#define LSM_REG_CTRL1_XL   0x10
#define LSM_REG_CTRL2_G    0x11
#define LSM_REG_OUTX_L_G   0x22

// BME280 Registers
#define BME280_REG_DIG_T1    0x88
#define BME280_REG_DIG_H1    0xA1
#define BME280_REG_DIG_H2    0xE1
#define BME280_REG_CHIPID    0xD0
#define BME280_REG_SOFTRESET 0xE0
#define BME280_REG_CTRL_HUM  0xF2
#define BME280_REG_STATUS    0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG    0xF5
#define BME280_REG_PRESS_MSB 0xF7

// Calibration data variables
uint16_t dig_T1;
int16_t  dig_T2, dig_T3;

uint16_t dig_P1;
int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

uint8_t  dig_H1, dig_H3;
int16_t  dig_H2, dig_H4, dig_H5;
int8_t   dig_H6;

// Global variable used to carry fine temperature over to pressure and humidity compensation
int32_t t_fine;

// Helper function to read an 8-bit register
uint8_t readRegister(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)addr, (uint8_t)1);
  return Wire.read();
}

// Helper function to read a 16-bit register (Little Endian)
uint16_t readRegister16LE(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)addr, (uint8_t)2);
  uint8_t lsb = Wire.read();
  uint8_t msb = Wire.read();
  return (msb << 8) | lsb;
}

// Helper function to write to an 8-bit register
void writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Reads the factory calibration arrays from the sensor
void readCalibrationData() {
  dig_T1 = readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1);
  dig_T2 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 2);
  dig_T3 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 4);

  dig_P1 = readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 6);
  dig_P2 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 8);
  dig_P3 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 10);
  dig_P4 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 12);
  dig_P5 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 14);
  dig_P6 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 16);
  dig_P7 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 18);
  dig_P8 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 20);
  dig_P9 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_T1 + 22);

  dig_H1 = readRegister(BME280_ADDR, BME280_REG_DIG_H1);
  dig_H2 = (int16_t)readRegister16LE(BME280_ADDR, BME280_REG_DIG_H2);
  dig_H3 = readRegister(BME280_ADDR, BME280_REG_DIG_H2 + 2);

  int8_t h4_msb = readRegister(BME280_ADDR, BME280_REG_DIG_H2 + 3);
  int8_t h4_lsb = readRegister(BME280_ADDR, BME280_REG_DIG_H2 + 4);
  int8_t h5_msb = readRegister(BME280_ADDR, BME280_REG_DIG_H2 + 5);

  dig_H4 = (h4_msb << 4) | (h4_lsb & 0x0F);
  dig_H5 = (h5_msb << 4) | (h4_lsb >> 4);
  dig_H6 = (int8_t)readRegister(BME280_ADDR, BME280_REG_DIG_H2 + 6);
}

float compensateTemperature(int32_t adc_T) {
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T / 100.0;
}

float compensatePressure(int32_t adc_P) {
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
  
  if (var1 == 0) return 0; // Avoid division by zero
  
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  return (float)p / 256.0;
}

float compensateHumidity(int32_t adc_H) {
  int32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((int32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + 
              ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * 
              (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + 
              ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  return (float)(v_x1_u32r >> 12) / 1024.0;
}

uint8_t _read_temp(float* temp, float* press, float* hum) {
  Wire.beginTransmission(BME280_ADDR);
  Wire.write(BME280_REG_PRESS_MSB);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)BME280_ADDR, (uint8_t)8);

  if (Wire.available() == 8) {
    uint8_t data[8];
    for (int i = 0; i < 8; i++) {
      data[i] = Wire.read();
    }

    // Combine raw bytes into 20-bit and 16-bit ADC values
    int32_t adc_P = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    int32_t adc_H = (data[6] << 8)  | data[7];

    *temp = compensateTemperature(adc_T);
    *press = compensatePressure(adc_P) / 100.0;
    *hum = compensateHumidity(adc_H);

    return 0; // Success
  } else {
    Serial.println("Failed to read data from BME280!");
    return 1; // Error code
  }
}

uint8_t _init_bme(void) {
    uint8_t chipId = readRegister(BME280_ADDR, BME280_REG_CHIPID);
    if (chipId != 0x60) {
        return 1; // Chip not found
    }
    
    readCalibrationData();
    
    writeRegister(BME280_ADDR, BME280_REG_CTRL_HUM, 0x01); // OS x1 Humidity
    writeRegister(BME280_ADDR, BME280_REG_CONFIG, 0xA0);   // Standby 1000ms
    writeRegister(BME280_ADDR, BME280_REG_CTRL_MEAS, 0x27); // OS x1 Temp/Press, Normal mode
    
    return 0; // Success
}

// Map the functions to the struct instance
bme_t bme = {
    .init = _init_bme,
    .read_temp = _read_temp,
};

// ==========================================
//               LSM6DSOX
// ==========================================

uint8_t _init_imu(void) {
    uint8_t whoAmI = readRegister(LSM_ADDR, LSM_REG_WHO_AM_I);
    if (whoAmI != 0x6C) {
        return 1; // Device not found
    }

    // Configure Accelerometer: 104Hz Data Rate, +/- 2g range
    writeRegister(LSM_ADDR, LSM_REG_CTRL1_XL, 0x40);

    // Configure Gyroscope: 104Hz Data Rate, +/- 250 dps range
    writeRegister(LSM_ADDR, LSM_REG_CTRL2_G, 0x40);

    return 0; // Success
}

uint8_t _read_imu(imu_data_t* data) {
    Wire.beginTransmission(LSM_ADDR);
    Wire.write(LSM_REG_OUTX_L_G);
    Wire.endTransmission(false);
    
    Wire.requestFrom((uint8_t)LSM_ADDR, (uint8_t)12);

    if (Wire.available() == 12) {
        uint8_t raw[12];
        for (int i = 0; i < 12; i++) raw[i] = Wire.read();

        int16_t raw_gx = (raw[1] << 8) | raw[0];
        int16_t raw_gy = (raw[3] << 8) | raw[2];
        int16_t raw_gz = (raw[5] << 8) | raw[4];

        int16_t raw_ax = (raw[7] << 8) | raw[6];
        int16_t raw_ay = (raw[9] << 8) | raw[8];
        int16_t raw_az = (raw[11] << 8) | raw[10];

        // Convert raw values using typical sensitivity modifiers
        data->ax = (raw_ax * 0.061f) / 1000.0f;
        data->ay = (raw_ay * 0.061f) / 1000.0f;
        data->az = (raw_az * 0.061f) / 1000.0f;

        data->gx = (raw_gx * 8.75f) / 1000.0f;
        data->gy = (raw_gy * 8.75f) / 1000.0f;
        data->gz = (raw_gz * 8.75f) / 1000.0f;

        return 0; // Success
    }
    return 1; // Error
}

// Map the IMU functions to the struct instance
imu_t imu = {
    .init = _init_imu,
    .read = _read_imu,
};
