#include <cansat.h>

void mission_setup() {
    Serial.println("WiFi kapcsolat teszt...");
    wifi.connect("IskolaHalozat", "jelszo123");
}

void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    if (wifi.connected()) {
        wifi.send("http://192.168.1.5/adatok", homerseklet, nyomas, paratartalom);
        Serial.println("WiFi adatok elküldve a dashboardra!");
    } else {
        Serial.println("Nincs WiFi kapcsolat...");
    }

    Serial.print("Hőmérséklet: ");
    Serial.print(homerseklet);
    Serial.println(" °C");
}
