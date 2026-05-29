#pragma once
#include <stdint.h>
#include "driver/uart.h"

class Radio {
public:
    bool init();
    bool send(const uint8_t* payload, uint8_t len);

private:
    static constexpr uint8_t SETTINGS_LEN = 5;
    static const char* _settings[SETTINGS_LEN];

    void _sendCmd(const char* cmd);
};
