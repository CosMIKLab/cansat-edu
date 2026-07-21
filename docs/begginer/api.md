# API Reference — Beginner Framework

See also: [overview.md](overview.md) (architecture) · [getting-started.md](getting-started.md) (first run, CLI reference)

Include everything with one line at the top of `mission.cpp`:

```cpp
#include <cansat.h>
```

This exposes six global objects: `sensors`, `radio`, `wifi`, `sd`, `led`, and `board` (internal).

---

## sensors

Reads the BMP580 (pressure/temperature), AHT20 (humidity/temperature), TMP102
(secondary temperature), LSM6DS3 (motion), and SAM-M8Q (GNSS) sensors. The object
presents them as one simple interface — you don't need to know which physical chip
answers which call.

### Environment

```cpp
float sensors.temperature()            // Temperature in degrees Celsius (from BMP580)
float sensors.pressure()               // Atmospheric pressure in hPa (from BMP580)
float sensors.humidity()               // Relative humidity in percent (%) (from AHT20)
float sensors.temperature_secondary()  // Secondary temperature reading (from TMP102)
```

Each call independently triggers a fresh read of its own sensor — there's no
required call order between `temperature()`, `pressure()`, `humidity()`, and
`temperature_secondary()`.

### Motion (accelerometer)

```cpp
float sensors.accel_x()  // Acceleration on X axis in g
float sensors.accel_y()  // Acceleration on Y axis in g
float sensors.accel_z()  // Acceleration on Z axis in g  (≈1.0 when flat)
```

Range: ±2 g. Sensitivity: 0.061 mg/LSB (LSM6DS3 ±2 g mode).

### Motion (gyroscope)

```cpp
float sensors.gyro_x()  // Rotation rate on X axis in degrees per second
float sensors.gyro_y()  // Rotation rate on Y axis in degrees per second
float sensors.gyro_z()  // Rotation rate on Z axis in degrees per second
```

Range: ±245 dps. Sensitivity: 8.75 mdps/LSB.

### GNSS

```cpp
bool sensors.gnss_available()          // true if the GNSS module answered on the I2C bus
const char* sensors.gnss_sentence()    // latest raw NMEA sentence, or "" if none yet
```

The GNSS module is optional hardware and its data is raw NMEA text (e.g.
`$GNRMC,...`) — this framework doesn't parse coordinates out of it for you. Turning a
sentence like `gnss_sentence()` into a latitude/longitude pair is a good intermediate-track
exercise.

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

Sends data over LoRa (E22-900M22S, SX1262 core, 868.1 MHz, SF7). The radio lives on its
own SPI bus — no physical switch to flip, the serial monitor and radio work at the same
time.

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
ESP32-S3 built-in WiFi — no extra hardware needed.

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

## led

Sets the panel's WS2816B status LED — useful for showing state without a serial monitor
attached (e.g. during flight).

```cpp
void led.begin()
```
Initialises the LED. Called automatically by the framework's `main.cpp` before
`mission_setup()` — you don't normally need to call this yourself.

```cpp
void led.set(uint8_t r, uint8_t g, uint8_t b)
```
Sets the LED color. Example: `led.set(0, 32, 0)` for a dim green.

```cpp
void led.clear()
```
Turns the LED off.

### Example

```cpp
if (tilt < 0.5) {
    led.set(255, 0, 0);  // red = tilted
} else {
    led.set(0, 32, 0);   // green = level
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
time_ms,temp_c,press_hpa,hum_pct,temp2_c,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps,nmea
2000,23.45,1013.25,48.70,23.40,0.000,0.000,1.000,0.00,0.00,0.00,""
```

Resulting `events.txt`:
```
[    2000] BOOT  session=S001
[    4312] apogee detected
```

> SD card must be FAT32 formatted. The SD card shares the SPI2 bus with the LoRa radio
> (separate chip-select pins) — `board.init()` sets both up before either device is used.

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
        led.set(255, 0, 0);
        sd.note("tilted more than 60 degrees!");
    } else {
        led.set(0, 32, 0);
    }
}
```
