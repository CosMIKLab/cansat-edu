#ifndef LSM6DS3_HPP
#define LSM6DS3_HPP

#include <stdint.h>

struct ImuData {
    float ax, ay, az;
    float gx, gy, gz;
};

class LSM6DS3 {
public:
    bool init();
    bool read(ImuData& data);
};

extern LSM6DS3 imu;

#endif /* LSM6DS3_HPP */
