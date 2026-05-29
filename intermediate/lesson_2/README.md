# 🛰️ Mérnöki Napló — 2. fejezet: Légköri mérés
## CSÜA Műszaki Divízió · BME280 Integráció

A Rendszertervező egy műszaki specifikációs lapot nyom a kezedbe.

*"A repülési engedélyhez igazolni kell, hogy az összes szenzor működik és kalibráció nélkül is legalább ±1°C / ±1 hPa pontosságú értékeket ad. A BME280 az elsőként ellenőrzendő chip. Az I2C buszon a 0x76 cím alatt érhető el — a driverünk ezt már kezeli, neked csak inicializálni és olvasni kell."*

Megnyitod a fejlécfájlt. Az `init()` visszatér `bool`-lal — ez fontos.

---

### 🎯 Tanulási célok

- Az `Adafruit_BME280` library API megértése (`begin()`, `readTemperature()`, `readPressure()`, `readHumidity()`)
- `bool` visszatérési értékek kezelése — hibavédelem
- `float` változók és lebegőpontos formátum (`%.2f`)
- Az I2C inicializálás folyamata (`begin()` belsejében mi történik)
- Mértékegység-konverzió: Pa → hPa (`/ 100.0F`)

---

### 💻 Kódmagyarázat

```cpp
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
```
Az `Adafruit_BME280` egy standard Arduino library manager csomag. A `bme` globális objektum bármely forrásfájlból elérhető.

```cpp
if (!bme.begin(BME280_ADDR)) {
    Serial.println("HIBA: BME280 nem elérhető!");
    while (1) delay(10);
}
```
A `begin()` `false`-t ad vissza, ha a chip nem válaszol az I2C buszon. Az `while (1)` végtelen ciklus leállítja a programot: soha ne folytassuk szenzor nélkül, mert értelmetlen adatot gyűjtenénk.

```cpp
float temp  = bme.readTemperature();
float press = bme.readPressure() / 100.0F;
float hum   = bme.readHumidity();
Serial.printf("T: %.2f °C  P: %.2f hPa  H: %.2f %%\n", temp, press, hum);
```

A `readPressure()` Pa (Pascal) egységben ad vissza értéket — ezt `/ 100.0F`-fel konvertáljuk hPa-ra. A `%.2f` két tizedesjegyre formázza az értéket. A `%%` egy literális százalékjelet ír ki (mert `%` önmagában formátumkarakter).

**Amit a `begin()` csinál belsőleg:**
1. Ellenőrzi a chip ID regisztert (0xD0 → érték 0x60)
2. Beolvassa a gyári kalibrációs együtthatókat (25 byte)
3. Beállítja a mérési módot: normál/folyamatos mérés

---

### 🚀 Kihívás

**1. szint:** Számítsd ki a közelítő magasságot a légnyomásból és írd ki:
```cpp
float altitude = (1013.25f - press) * 8.5f;
Serial.printf("Magasság (közelítő): %.1f m\n", altitude);
```
Mi a hiányossága ennek a módszernek? (Gondolj a hőmérséklet hatására.)

**2. szint:** Módosítsd a kiírást úgy, hogy a hőmérséklet mellé írja ki, „meleg" (`>25°C`), „normális" (`10–25°C`) vagy „hideg" (`<10°C`) a körülmény. Írd meg `if-else if-else` szerkezettel.

**3. szint — gondolkodós:** Mi a különbség a `float&` és a `float` paramétertípus között? Miért nem `return`-öl három értéket a `read()`? Hogyan oldanád meg C++11-es `struct`-tal? (A 4. fejezetben ezt fogjuk csinálni!)

> **Tipp:** `./cansat new` visszaállítja a sablont.

---

### 🏆 Megszerzett jelvény

**🌡️ BME280 Kalibrációs Tanúsítvány**
*A légköri szenzor inicializálva, kalibrálva és adatot szolgáltat. A repülési engedélyhez szükséges első szenzorteszt: ÁTMENT.*

---

### ➡️ Következő fejezet

A 3. fejezetben a légnyomásból pontos magasságot számolsz — a valódi barometrikus formulával. Megismered, miért kell talaj-kalibrációt végezni, és hogyan befolyásolja a hőmérséklet a mérés pontosságát.
