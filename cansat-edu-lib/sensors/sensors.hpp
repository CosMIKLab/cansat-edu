#ifndef SENSORS_HPP
#define SENSORS_HPP

class Sensors {
public:
    void begin();
    bool bmp580_present();
    bool aht20_present();
    bool tmp102_present();
    bool imu_present();
    float temperature();
    float pressure();
    float humidity();
    float temperature_secondary();
    float accel_x();
    float accel_y();
    float accel_z();
    float gyro_x();
    float gyro_y();
    float gyro_z();
    bool  gnss_available();
    const char* gnss_sentence();
};

extern Sensors sensors;

#endif /* SENSORS_HPP */
