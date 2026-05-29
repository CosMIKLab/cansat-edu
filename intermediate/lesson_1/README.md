# 🛰️ Mérnöki Napló — 1. fejezet: Rendszerindítás
## CSÜA Műszaki Divízió · Rendszer-aktiválás

A beléptetőkártyád beolvasva. A Műszaki Divízió folyosóján a Rendszertervező már vár.

*"Üdvözöljük a CSÜA Műszaki Divíziójában. A feladatod az lesz, hogy megírd a CanSat fedélzeti szoftverét az indulástól a landolásig. De most még a legelső lépésnél tartunk: a hardver aktiválásánál. Mielőtt bármelyik szenzort érintenéd, a fedélzeti számítógépnek el kell indulnia és kommunikálnia kell a földi állomással."*

Kinyitod a `src/mission.cpp` fájlt. Az első feladat egyszerűnek tűnik — de pontosan kell csinálni.

---

### 🎯 Tanulási célok

- A fejlesztői környezet felépítése (PlatformIO, Arduino framework)
- A `setup()` és `loop()` függvény szerepe és különbsége
- `#ifndef` include guard alkalmazása fejlécfájlokban
- `Serial.println()` és `Serial.printf()` használata
- A `millis()` függvény — az előre görgő időbélyeg

---

### 💻 Kódmagyarázat

```cpp
#include <Arduino.h>
#include "board.hpp"
```

Minden forrásfájl ezzel kezdődik. Az `<Arduino.h>` az Arduino keretrendszer alaptárkönyvtára. A `"board.hpp"` a mi saját hardver-inicializáló modulunk — ez konfigurálja a Serial portot és az I2C buszt.

```cpp
void setup() {
    board.init();
    ...
}
```

A `setup()` **egyszer fut le**, közvetlenül bekapcsolás után. A `board.init()` elindítja a soros portot (`115200 baud`) és konfigurálja az I2C buszt (GPIO4 SDA, GPIO5 SCL, 400 kHz).

```cpp
void loop() {
    Serial.printf("[%8lu ms] Fedélzeti rendszer — normális\n", millis());
    delay(2000);
}
```

A `loop()` **folyamatosan ismétlődik**. A `%8lu` formátumkód egy 8 karakter szélességre jobbra igazított előjel nélküli számot ír ki. A `millis()` visszaadja a bekapcsolás óta eltelt ezredmásodpercek számát — ez lesz a telemetria időbélyege.

**A `board.hpp` fejlécfájl felépítése:**
```cpp
#ifndef BOARD_HPP
#define BOARD_HPP
// ... tartalom ...
#endif /* BOARD_HPP */
```
Ez az **include guard** mintázat. Megakadályozza, hogy ugyanazt a fejlécet kétszer fordítsuk le — ami szimbólumduplázási hibát okozna. A `#pragma once` egyszerűbb, de nem szabványos; az intermediate szinten a portábilis `#ifndef` változatot használjuk.

---

### 🚀 Kihívás

**1. szint:** Módosítsd a `setup()` startup üzenetét — szerepeljen benne a neved és a csapat neve. A blokk legyen 30 karakter széles, szimmetrikus.

**2. szint:** A `loop()`-ban ne csak az eltelt időt írd ki, hanem azt is, hány másodperce fut a rendszer (`millis() / 1000`). Formátum: `[  2500 ms | 2 s] ...`

**3. szint — gondolkodós:** Mi történik, ha a `delay(2000)` helyett `delay(0)`-t írsz? Hány üzenet jelenik meg másodpercenként? Miért problémás ez a valódi alkalmazásban? (Előkészítés a 12. fejezethez.)

> **Tipp:** Ha visszaállítanád a kiindulópontot: `./cansat new`

---

### 🏆 Megszerzett jelvény

**⚡ Rendszer-aktiváló Mérnöki Jelvény**
*A fedélzeti számítógép elindul, kommunikál és időbélyeget kezel. Az első aktiválási teszt sikeres — a Műszaki Divízió felvett a csapatba.*

---

### ➡️ Következő fejezet

A 2. fejezetben csatlakoztatod az első igazi szenzort: a **BME280** légköri mérőchipet. Megtanulod, hogyan inicializálj egy I2C perifériát, és hogyan olvass ki hőmérsékletet, légnyomást és páratartalmat egyetlen függvényhívással.
