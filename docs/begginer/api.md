# API Reference — Beginner Framework

See also: [overview.md](overview.md) (architecture) · [getting-started.md](getting-started.md) (first run, CLI reference)

Include everything with one line at the top of `mission.cpp`:

```cpp
#include <cansat.h>
```

This exposes five global objects: `sensors`, `radio`, `wifi`, `sd`, and `board` (internal).

---

## sensors

Reads the BME280 (environment) and LSM6DSOX (motion) sensors.

### Environment

```cpp
float sensors.temperature()  // Temperature in degrees Celsius
float sensors.pressure()     // Atmospheric pressure in hPa
float sensors.humidity()     // Relative humidity in percent (%)
```

Calling `sensors.temperature()` triggers a fresh read of all three values. Call it first; `pressure()` and `humidity()` return the values cached from that read.

### Motion (accelerometer)

```cpp
float sensors.accel_x()  // Acceleration on X axis in g
float sensors.accel_y()  // Acceleration on Y axis in g
float sensors.accel_z()  // Acceleration on Z axis in g  (≈1.0 when flat)
```

Range: ±2 g. Sensitivity: 0.061 mg/LSB (LSM6DSOX ±2 g mode).

### Motion (gyroscope)

```cpp
float sensors.gyro_x()  // Rotation rate on X axis in degrees per second
float sensors.gyro_y()  // Rotation rate on Y axis in degrees per second
float sensors.gyro_z()  // Rotation rate on Z axis in degrees per second
```

Range: ±250 dps. Sensitivity: 8.75 mdps/LSB.

### Example

```cpp
float temp     = sensors.temperature();
float pressure = sensors.pressure();
float humidity = sensors.humidity();
float tilt     = sensors.accel_z();

Serial.print("Temp: "); Serial.print(temp); Serial.println(" C");
```

---

## radio

Sends data over LoRa (RN2483, 868.1 MHz, SF7). The physical UART switch on the board must be in the **Radio** position during flight.

```cpp
void radio.send(float a, float b, float c)
```
Encodes three floats as a comma-separated string and transmits it as a LoRa packet.
Example: `radio.send(23.4, 1013.2, 48.7)` → packet payload `"23.40,1013.20,48.70"`.

```cpp
void radio.send(String msg)
```
Transmits a custom string as a LoRa packet.
Example: `radio.send("launched!")`.

> **Duty cycle:** The 868 MHz EU band enforces 1% duty cycle. At SF7 (~50 ms packets) the safe maximum rate is roughly one packet every 5 seconds. The 2-second loop is within this limit.

### Example

```cpp
radio.send(temp, pressure, humidity);
radio.send("max altitude reached");
```

---

## wifi

Connects to a WiFi network and sends data to an HTTP endpoint.  
ESP8266 built-in WiFi — no extra hardware needed.

```cpp
void wifi.connect(const char* ssid, const char* password)
```
Connects to a WiFi network. Blocks up to 10 seconds. Prints the IP address to Serial on success. Call this in `mission_setup()`.

```cpp
void wifi.send(const char* url, float a, float b, float c)
```
Sends a GET request to `url?a=<a>&b=<b>&c=<c>`. Fire-and-forget — no response is parsed. Does nothing if not connected.

```cpp
bool wifi.connected()
```
Returns `true` if currently connected to WiFi.

### Example

```cpp
void mission_setup() {
    wifi.connect("SchoolNet", "password123");
}

void mission_loop() {
    float t = sensors.temperature();
    float p = sensors.pressure();
    float h = sensors.humidity();

    wifi.send("http://192.168.1.5/data", t, p, h);
    // → GET http://192.168.1.5/data?a=23.40&b=1013.20&c=48.70
}
```

---

## sd

Logs data to the SD card. Each power-on creates a new session folder (`S001`, `S002`, …) containing two files:

- `telem.csv` — one row per `sd.log()` call
- `events.txt` — one line per `sd.note()` call with millisecond timestamps

```cpp
void sd.log(float a, float b, float c)
```
Appends a CSV row with the current timestamp and three sensor values.
Column order: `time_ms, temp_c, press_hpa, hum_pct` (a, b, c map to temp, pressure, humidity).

```cpp
void sd.note(const char* msg)
```
Appends a timestamped line to `events.txt`. Use this to mark events like launch, apogee, or recovery.

### Example

```cpp
sd.log(temp, pressure, humidity);
sd.note("apogee detected");
```

Resulting `telem.csv`:
```
time_ms,temp_c,press_hpa,hum_pct,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps
2000,23.45,1013.25,48.70,0.000,0.000,1.000,0.00,0.00,0.00
```

Resulting `events.txt`:
```
[    2000] BOOT  session=S001
[    4312] apogee detected
```

> SD card must be FAT32 formatted. GPIO15 is the SPI CS pin — it requires a 10 kΩ pull-down to GND for proper ESP8266 boot behaviour (handled by the PCB).

---

## Full example

```cpp
#include <cansat.h>

void mission_setup() {
    wifi.connect("MyNetwork", "secret");
}

void mission_loop() {
    float temp     = sensors.temperature();
    float pressure = sensors.pressure();
    float humidity = sensors.humidity();
    float tilt     = sensors.accel_z();

    Serial.print("T="); Serial.print(temp);
    Serial.print(" P="); Serial.print(pressure);
    Serial.print(" H="); Serial.println(humidity);

    radio.send(temp, pressure, humidity);
    wifi.send("http://192.168.1.5/cansat", temp, pressure, humidity);
    sd.log(temp, pressure, humidity);

    if (tilt < 0.5) {
        sd.note("tilted more than 60 degrees!");
    }
}
```
