# Lesson 12 — WiFi Stack & HTTP Client

## Learning Objectives
- Initialise the ESP32-S3 WiFi driver and connect to a WPA2 network
- Use the ESP-IDF event loop to handle asynchronous WiFi events
- Send sensor data to a remote server with `esp_http_client`

## Key Concepts

### WiFi Initialisation (Station Mode)
```c
// 1. NVS (required for WiFi calibration data)
nvs_flash_init();

// 2. TCP/IP stack and default event loop
esp_netif_init();
esp_event_loop_create_default();
esp_netif_create_default_wifi_sta();

// 3. WiFi driver
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
esp_wifi_init(&cfg);

// 4. Register event handlers
esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, handler, NULL, NULL);
esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, handler, NULL, NULL);

// 5. Configure and start
wifi_config_t wifi_cfg = { .sta = { .ssid = "SSID", .password = "PASS" } };
esp_wifi_set_mode(WIFI_MODE_STA);
esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
esp_wifi_start();
```

### Event Handler Pattern
The WiFi stack is asynchronous. Events fired:
- `WIFI_EVENT_STA_START` → call `esp_wifi_connect()`
- `WIFI_EVENT_STA_DISCONNECTED` → retry with `esp_wifi_connect()`
- `IP_EVENT_STA_GOT_IP` → connected! Signal the application task

### HTTP Client
```c
esp_http_client_config_t cfg = { .url = "http://host/path?a=1" };
esp_http_client_handle_t client = esp_http_client_init(&cfg);
esp_err_t err = esp_http_client_perform(client);
int status = esp_http_client_get_status_code(client);
esp_http_client_cleanup(client);
```

## Build & Flash
Update `WIFI_SSID`, `WIFI_PASS`, and `SERVER_URL` in `src/main.cpp`, then:
```
pio run -t upload && pio device monitor
```
You can test without a real server using `python3 -m http.server 8080` on your laptop and pointing `SERVER_URL` to your laptop's IP.
