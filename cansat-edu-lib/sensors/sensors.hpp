#ifndef SENSORS_HPP
#define SENSORS_HPP

class Sensors {
public:
    void begin();
    float temperature();
    float pressure();
    float humidity();
    float accel_x();
    float accel_y();
    float accel_z();
    float gyro_x();
    float gyro_y();
    float gyro_z();
};

extern Sensors sensors;

#endif /* SENSORS_HPP */
