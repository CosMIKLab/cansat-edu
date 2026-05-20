# 🛰️ Küldetési Napló — 3. fejezet: Légköri megfigyelés
## CSÜA Küldetésközpont · Érzékelő Teszt II.

A hőmérő rendszer tesztje sikerrel zárult — az adatok pontosak. Most a légköri osztály egy teljesebb adatcsomagot kér.

A tudományos vezető a nagy kivetítőre mutat, ahol egy légköri profil látható: *"A CanSat repülés közben folyamatosan rögzíti a légkör állapotát. A hőmérséklet önmagában kevés — szükségünk van a **légnyomásra** is, hogy kiszámíthassuk a magasságot, és a **páratartalomra** a meteorológiai modellekhez."*

Szerencsére a BME280 szenzor mindhármat tudja — egyetlen chipben van a hőmérő, a barométer és a páramérő. A te feladatod: mind a három értéket kiolvasni és szépen megjeleníteni a monitoron.

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan használj egyszerre **több változót** egy programban
- Mi az a **légnyomás** (hPa) és **páratartalom** (%)
- Hogyan írj ki szépen formázott, igazított adatokat a monitorra

---

### 💻 A kódod magyarázata

```cpp
void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();
    ...
}
```

Három `float` változó, három érték. Fontos: **mindig a `sensors.temperature()`-t hívd meg először!** Ez frissíti mindhárom értéket a szenzorban. Utána a `pressure()` és `humidity()` már az éppen mért adatot adja vissza.

```cpp
    Serial.println("--- Légköri adatok ---");
    Serial.print("Hőmérséklet:  "); Serial.print(homerseklet);  Serial.println(" °C");
    Serial.print("Légnyomás:    "); Serial.print(nyomas);        Serial.println(" hPa");
    Serial.print("Páratartalom: "); Serial.print(paratartalom);  Serial.println(" %");
```

Minden adatsornak 3 része van:
1. **Felirat** (`Serial.print`) — pl. `"Hőmérséklet:  "` — nem vált sort
2. **Szám** (`Serial.print(változó)`) — a mért érték — nem vált sort
3. **Egység** (`Serial.println`) — pl. `" °C"` — **sort vált**

Az extra szóközök a feliratoknál (`"  "`) azért vannak, hogy a számok szépen egy oszlopba essenek — próbáld ki a monitoron!

**Mértékegységek:**
- `°C` — Celsius-fok (hőmérséklet)
- `hPa` — hektopascal (légnyomás); tengerszinten kb. 1013 hPa, magasabban kevesebb
- `%` — százalék (páratartalom); 0% = száraz levegő, 100% = köd/eső

---

### 🚀 Kihívás

**1. szintű kihívás:** A légnyomásból ki lehet számítani a közelítő magasságot: minden 8.5 méteres emelkedésnél kb. 1 hPa-val csökken. Add hozzá ezt a sort a kiírás után:
```cpp
Serial.print("Közelítő magasság: ");
Serial.print((1013.25 - nyomas) * 8.5);
Serial.println(" m");
```
Figyeld meg az értéket — mennyire pontos?

**2. szintű kihívás:** Írd ki az adatokat más sorrendben: először páratartalom, aztán hőmérséklet, végül légnyomás. Mi változik?

**3. szintű kihívás — gondolkodós:** Mi történik, ha kihagyod a `sensors.temperature()` hívást, és csak `sensors.pressure()`-t hívsz? Próbáld ki! Mire következtetsz?

---

### 🏆 Megszerzett jelvény

**🌤️ Meteorológiai Technikusi Jelvény**
*A BME280 mind a három csatornája aktív és kalibrált. A légköri osztály megkapta a teljes szenzor-adatcsomagot. Kiváló munka!*

---

### ➡️ Következő fejezet

A 4. fejezetben egy teljesen más típusú szenzort kapcsolsz be: a **gyorsulásmérőt**. Ez nem a levegőt méri, hanem a CanSat **mozgását** — forgatás, rázás, gyorsulás. Kiderül, mit jelent a "g egység", és hogyan lehet érzékelni, ha a műhold eldől.
