#ifndef AHT20_HPP
#define AHT20_HPP

#include <stdint.h>

class AHT20 {
public:
    bool init();
    bool read(float& temp, float& hum);
};

extern AHT20 aht20;

#endif /* AHT20_HPP */
