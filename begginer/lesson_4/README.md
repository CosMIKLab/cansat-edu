# 🛰️ Küldetési Napló — 4. fejezet: Mozgásérzékelés
## CSÜA Küldetésközpont · IMU Ellenőrzés

A légköri szenzorok rendben vannak. De egy CanSat repülés közben nem csak az időjárást méri — gyorsan forog, a rakéta erősen felgyorsítja, majd az ejtőernyős ereszkedés alatt lassan ringatja. A mérnöki osztály ezért kéri a **mozgásérzékelő** tesztelését is.

*"Az LSM6DSOX chip a CanSat belsejében van"*, magyarázza a mérnök. *"Három tengelyen méri a gyorsulást. Gondolj rá úgy, mint a telefonodban lévő szenzorra — az is tudja, hogy fekteted vagy tartod. A CanSat esetén ez megmutatja, merre áll, és mekkora erőt érez."*

A "g" egység a Föld gravitációs gyorsulása (9.81 m/s²). Ha a CanSat vízszintesen fekszik, a Z-tengely értéke **kb. 1.0 g** — mert a Föld vonzza lefelé. Repülés közben ez ugrálhat és változhat.

---

### 🎯 Ebben a leckében megtanulod:

- Mi az a **gyorsulásmérő** (accelerometer) és a **g egység**
- Hogyan olvasd le a három tengely értékét (`accel_x`, `accel_y`, `accel_z`)
- Mit jelent fizikailag, ha egy tengely értéke 0, 1 vagy nagyobb

---

### 💻 A kódod magyarázata

```cpp
void mission_loop() {
    float ax = sensors.accel_x();
    float ay = sensors.accel_y();
    float az = sensors.accel_z();
    ...
}
```

Három változó, három tengely:
- **`ax`** — X-tengely (balra-jobbra dőlés)
- **`ay`** — Y-tengely (előre-hátra dőlés)
- **`az`** — Z-tengely (föl-le, gravitáció irány)

```cpp
    Serial.println("--- Gyorsulásmérő ---");
    Serial.print("X: "); Serial.print(ax); Serial.println(" g");
    Serial.print("Y: "); Serial.print(ay); Serial.println(" g");
    Serial.print("Z: "); Serial.print(az); Serial.println(" g");
    Serial.println("(Z = 1.0 amikor a CanSat vízszintes)");
```

**Várható értékek álló CanSatnál:**
| Helyzet | X | Y | Z |
|---------|---|---|---|
| Vízszintes (lap) | ~0 | ~0 | ~1 |
| 90°-ban billentve | ~1 | ~0 | ~0 |
| Fejjel lefelé | ~0 | ~0 | ~-1 |

Próbáld ki: fordítsd meg, döntsd meg a CanSat-ot miközben fut a monitor — és figyeld, hogyan változnak az értékek!

**A szenzor határtartománya:** ±2 g — tehát –2.0 és +2.0 g között mér megbízhatóan. Rakétaindításkor (akár 10–20 g) a szenzor telítésbe mehet, de ez normális.

---

### 🚀 Kihívás

**1. szintű kihívás:** Add hozzá a **giroszkóp** értékeket is! A giroszkóp forgási sebességet mér (fok/másodperc). Próbáld ki:
```cpp
float gx = sensors.gyro_x();
float gy = sensors.gyro_y();
float gz = sensors.gyro_z();
Serial.print("Forgás Z: "); Serial.print(gz); Serial.println(" dps");
```
Forgasd meg a CanSat-ot és figyeld az értéket!

**2. szintű kihívás:** A dőlésszög kiszámítható a gyorsulásból. Ha `az` közel van 1.0-hoz, a CanSat majdnem vízszintes. Ha `az` kisebb, mint 0.7, kb. 45 foknál jobban dől. Írd ki szövegesen, hogy "Vízszintes" vagy "Megdőlt":
```cpp
if (az > 0.7) {
    Serial.println("Helyzet: vízszintes");
} else {
    Serial.println("Helyzet: megdőlt!");
}
```
*(Ez előkészület a következő leckéhez!)*

**3. szintű kihívás — gondolkodós:** Miért pont a Z-tengely mutatja a gravitációt? Hogyan helyezkedik el a szenzor a panelen?

---

### 🏆 Megszerzett jelvény

**⚖️ Mozgásérzékelő Technikusi Jelvény**
*Az LSM6DSOX IMU chip mindhárom gyorsulásmérő tengelye aktív. A mérnöki osztály megerősítette, hogy a CanSat helyzet-érzékelése működőképes.*

---

### ➡️ Következő fejezet

A 5. fejezetben a CanSat "okos" lesz: **döntéseket hoz** a mért adatok alapján. Ha a hőmérséklet túl magas, figyelmeztet. Ha megdőlt, riaszt. Megtanulod az `if` / `else` szerkezetet — a programozás egyik legalapvetőbb eszközét.
