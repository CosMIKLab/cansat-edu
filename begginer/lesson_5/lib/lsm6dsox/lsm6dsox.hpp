#pragma once
#include <stdint.h>

struct ImuData {
    float ax, ay, az;  // Accelerometer in g
    float gx, gy, gz;  // Gyroscope in dps
};

class LSM6DSOX {
public:
    bool init();
    bool read(ImuData& data);
private:
    bool _ready = false;
};

extern LSM6DSOX imu;
