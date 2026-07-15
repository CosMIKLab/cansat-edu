#ifndef GNSS_HPP
#define GNSS_HPP

#include <stdint.h>

struct GnssData {
    bool     present      = false;
    uint16_t pendingBytes = 0;
    char     nmea[96]     = {0};
};

class GNSS {
public:
    bool init();
    bool read(GnssData& data);
};

extern GNSS gnss;

#endif /* GNSS_HPP */
