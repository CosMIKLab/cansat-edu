#ifndef RADIO_HPP
#define RADIO_HPP

#include <stdint.h>

class Radio {
public:
    bool init();
    bool present() const;
    bool send(const uint8_t* payload, uint8_t len);
    int  receive(uint8_t* buffer, uint8_t maxLen, uint32_t timeoutMs);

private:
    bool _ready = false;

    void configureModem();
};

extern Radio radio;

#endif /* RADIO_HPP */
