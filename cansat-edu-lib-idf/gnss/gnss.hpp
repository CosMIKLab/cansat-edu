#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

struct GnssData {
    bool     present;
    uint16_t pendingBytes;
    char     nmea[96];
};

// u-blox SAM-M8Q GNSS receiver, DDC (I2C) streaming interface. This driver
// does NOT parse coordinates out of the NMEA text — it hands you the raw
// sentence, matching the scope of the reference firmware it was ported from.
class GNSS {
public:
    bool init(i2c_master_bus_handle_t bus);
    bool read(GnssData& data);

private:
    i2c_master_dev_handle_t _dev     = nullptr;
    bool                    _present = false;
    char                    _streamWindow[192] = {0};
    uint8_t                 _streamLen = 0;

    esp_err_t _readReg(uint8_t reg, uint8_t* data, size_t len);
    void      _appendStream(const uint8_t* data, uint8_t len);
    void      _updateLatestNmea(GnssData& out) const;
};
