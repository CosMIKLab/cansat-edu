# 🛰️ Mérnöki Napló — 4. fejezet: Mozgásérzékelés
## CSÜA Műszaki Divízió · IMU Integráció

A mérnöki osztályról egy e-mail érkezik: *"A repülési profil elemzéséhez szükségünk van a CanSat orientációjára és gyorsulására. Az LSM6DSOX chip beépítve, I2C-n érhető el (0x6A). Konfiguráld 104 Hz-es ODR-re és ±2g tartományra — ezek az optimális értékek a mi rakétaindítási gyorsulásunkhoz."*

Megnézed a chip adatlapját. 12 byte burst-read, kalibrációs együtthatók — de az Adafruit library mindezt elrejti. Neked csak a `sensors_event_t` struktúrával kell dolgoznod.

---

### 🎯 Tanulási célok

- Az `Adafruit LSM6DS` library API: `begin_I2C()`, `getEvent()`
- A `sensors_event_t` struktúra és a `acceleration` / `gyro` almezők
- Gyorsulás (`ax, ay, az`) és szögsebesség (`gx, gy, gz`) fizikai jelentése
- A g-egység és dps (degrees per second) mértékegység
- Miért mér Z-tengelyen ~1.0g álló CanSatnál
- Két szenzor egyidejű kezelése

---

### 💻 Kódmagyarázat

```cpp
#include <Adafruit_LSM6DSOX.h>
Adafruit_LSM6DSOX imu;
```
Az `Adafruit LSM6DS` standard Arduino library manager csomag, amely az LSM6DSOX chippel is kompatibilis.

```cpp
imu.begin_I2C(LSM_ADDR);
```
Inicializálja a chipet I2C-n a megadott cím alatt. `false`-t ad vissza, ha a chip nem érhető el.

```cpp
sensors_event_t accel, gyro, temp_e;
imu.getEvent(&accel, &gyro, &temp_e);
float ax = accel.acceleration.x / 9.80665f;  // m/s² → g
float az = accel.acceleration.z / 9.80665f;
float gx = gyro.gyro.x * (180.0f / M_PI);   // rad/s → dps
```
A `getEvent()` pointer-rel veszi át a három event struktúrát. A `acceleration` mező `m/s²` egységben tárolja az adatot — `/ 9.80665f`-fel konvertálunk `g`-be. A `gyro` mező `rad/s`-ban van — `* (180/π)`-vel dps-be.

**Várható értékek (álló CanSat, Z-tengely felfelé mutat):**
| Kifejezés | Érték | Magyarázat |
|---|---|---|
| `accel.acceleration.x / 9.80665f` | ~0.0 g | Nincs vízszintes gyorsulás |
| `accel.acceleration.z / 9.80665f` | ~1.0 g | Gravitáció — a chip „érzi" |
| `gyro.gyro.x * (180/π)` | ~0.0 dps | Nem forog |

**ODR (Output Data Rate) = 104 Hz** — a chip 104-szer mér másodpercenként. Mi 2 Hz-en olvassuk ki (500ms-enként), tehát minden olvasás a legutolsó mérést adja vissza.

```cpp
Serial.printf("Accel [g]  X: %.3f  Y: %.3f  Z: %.3f\n",
    accel.acceleration.x / 9.80665f,
    accel.acceleration.y / 9.80665f,
    accel.acceleration.z / 9.80665f);
```
A `%.3f` három tizedesjegy — gyorsulásnál ez szükséges, mert a mozgás apró változásokat mutat.

---

### 🚀 Kihívás

**1. szint:** Forgasd el a CanSat-ot 90 fokkal a X-tengely körül (fordítsd az oldalára). Mit mutatnak az értékek? Dokumentáld egy táblázatban: melyik tengely mutat 1.0g-t melyik orientációban.

**2. szint:** Számítsd ki a teljes gyorsulásvektort: `|a| = sqrt(ax² + ay² + az²)`. Álló CanSatnál ez mindig ~1.0g kell legyen. Írd ki mellette a eltérést: `delta = |a| - 1.0`. (Szükséges: `#include <math.h>` és `sqrtf()`)

**3. szint — gondolkodós:** Ha a CanSat szabadesésben van (pl. az ejtőernyő nem nyílt ki), mi lesz az `az` értéke? Miért? Hogyan lehetne ezt detektálni? (Ez lesz a 10. fejezet egyik kulcsa.)

---

### 🏆 Megszerzett jelvény

**⚖️ IMU Integráló Jelvény**
*Az LSM6DSOX mindhárom gyorsulás- és szögsebességtengelye kalibrálva és tesztelve. A mozgásérzékelő alrendszer repülésre kész.*

---

### ➡️ Következő fejezet

Az 5. fejezetben a nyers gyorsulásadatokból **valós fizikai mennyiségeket** számolsz: vektormagnitudót és dőlésszöget. Megismered a `sqrtf()` és `acosf()` függvényeket, és megérted, mikor „hazudik" a gyorsulásmérő.
