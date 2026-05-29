#ifndef LSM6DSOX_HPP
#define LSM6DSOX_HPP

#include <stdint.h>

struct ImuData {
    float ax, ay, az;
    float gx, gy, gz;
};

class LSM6DSOX {
public:
    bool init();
    bool read(ImuData& data);
};

extern LSM6DSOX imu;

#endif /* LSM6DSOX_HPP */
