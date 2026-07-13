#ifndef BME280_HPP
#define BME280_HPP

#include <stdint.h>

class BME280 {
public:
    bool init();
    bool read(float& temp, float& press, float& hum);
};

extern BME280 bme;

#endif /* BME280_HPP */
