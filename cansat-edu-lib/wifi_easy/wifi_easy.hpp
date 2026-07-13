#ifndef WIFI_EASY_HPP
#define WIFI_EASY_HPP

#include <Arduino.h>

class WiFiEasy {
public:
    void connect(const char* ssid, const char* password);
    void send(const char* url, float a, float b, float c);
    bool connected();
};

extern WiFiEasy wifi;

#endif /* WIFI_EASY_HPP */
