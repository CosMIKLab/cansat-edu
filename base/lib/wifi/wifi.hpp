#pragma once
#include <Arduino.h>

class WiFiModule {
public:
    void connect(const char* ssid, const char* password);
    void send(const char* url, float a, float b, float c);
    bool connected();
};

extern WiFiModule wifi;
