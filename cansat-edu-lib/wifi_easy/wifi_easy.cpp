#include "wifi_easy.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

void WiFiEasy::connect(const char* ssid, const char* password) {
    Serial.print("WiFi: connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    uint8_t tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 20) {
        delay(500);
        Serial.print('.');
        tries++;
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi: connected, IP=");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi: connection failed");
    }
}

bool WiFiEasy::connected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiEasy::send(const char* url, float a, float b, float c) {
    if (!connected()) return;
    char full[128];
    snprintf(full, sizeof(full), "%s?a=%.2f&b=%.2f&c=%.2f", url, a, b, c);
    WiFiClient client;
    HTTPClient http;
    http.begin(client, full);
    http.GET();
    http.end();
}

WiFiEasy wifi;
