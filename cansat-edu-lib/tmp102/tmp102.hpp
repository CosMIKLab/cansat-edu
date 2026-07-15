#ifndef TMP102_HPP
#define TMP102_HPP

#include <stdint.h>

class TMP102 {
public:
    bool init();
    bool read(float& temp);
};

extern TMP102 tmp102;

#endif /* TMP102_HPP */
