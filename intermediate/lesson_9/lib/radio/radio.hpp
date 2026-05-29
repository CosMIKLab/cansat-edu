#ifndef RADIO_HPP
#define RADIO_HPP
#include <stdint.h>

static constexpr uint8_t RADIO_SETTINGS_LEN = 5;

class Radio {
public:
    bool init();
    bool send(const uint8_t* payload, uint8_t len);

private:
    static const char* _settings[RADIO_SETTINGS_LEN];
};

extern Radio radio;

#endif /* RADIO_HPP */
