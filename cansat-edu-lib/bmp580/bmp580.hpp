#ifndef BMP580_HPP
#define BMP580_HPP

#include <stdint.h>

class BMP580 {
public:
    bool init();
    bool read(float& temp, float& press);
};

extern BMP580 bmp580;

#endif /* BMP580_HPP */
