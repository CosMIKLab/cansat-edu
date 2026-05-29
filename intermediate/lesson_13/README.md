# 🛰️ Mérnöki Napló — 13. fejezet: Jelfeldolgozás
## CSÜA Műszaki Divízió · Zajszűrés és Simítás

A nyers telemetriai adatokat átnézve a Rendszertervező egy grafikont mutat: *"Ez a hőmérséklet-mérés zajossága ±0.3°C — elfogadható. De az az-tengely zaját nézzük: ±0.02g, ami látszólag kis szám, de 104 Hz-es ODR-nél rengeteg ingadozás van. A tetőpont-detektor és a felszállás-detektor tévedhet emiatt. Szűrőkkel ezt meg kell oldani."*

Két szűrőt implementálsz: EMA-t a valós idejű folyamatos szignálokhoz, és csúszó átlagot a hőmérséklethez.

---

### 🎯 Tanulási célok

- Exponenciális mozgó átlag (EMA) vs. egyszerű csúszó átlag (SMA) összehasonlítása
- Gyűrűpuffer SMA implementálása `float` tömbben
- Alpha paraméter empirikus tuningolása Serial output alapján
- Szűrt vs. nyers értékek egymás melletti megjelenítése
- Szűrési késés (latency) és zajcsökkentés trade-off

---

### 💻 Kódmagyarázat

**EMA (Exponential Moving Average):**
```cpp
static constexpr float ALPHA_P = 0.2f;
ema_p = ALPHA_P * p_raw + (1.0f - ALPHA_P) * ema_p;
```
Az EMA exponenciálisan súlyozza a múltbeli értékeket — a legutóbbi mintának van a legnagyobb hatása, de az összes korábbi is beleszámít. Memóriaigénye: egyetlen `float` változó!

**SMA (Simple Moving Average) — gyűrűpufferes implementáció:**
```cpp
static constexpr uint8_t TBUF = 8;
static float tempBuf[TBUF] = {};
static float tSum = 0.0f;

float updateTempAvg(float sample) {
    tSum -= tempBuf[tIdx];        // kivon a puffer legrégebbi elemét
    tempBuf[tIdx] = sample;       // beírja az újat
    tSum += sample;               // hozzáadja az összeghez
    tIdx = (tIdx + 1) % TBUF;    // ciklikusan lépteti az indexet
    return tSum / TBUF;           // átlag visszaadása
}
```

Az SMA minden mintát **egyformán súlyoz** az ablakban. Memóriaigénye: `TBUF × sizeof(float)` = 32 byte — 8 mintánál. Pontosabb mint az EMA, de lassabb reagálású.

**Összehasonlítás:**
| Jellemző | EMA | SMA (8 minta) |
|---|---|---|
| Memória | 4 byte | 32 byte |
| Reagálás | Azonnali (α-val skálázott) | Lassabb |
| Zaj-szűrés | Közepes | Egyenletesebb |
| Implementáció | 1 sor | Gyűrűpuffer |

```cpp
Serial.printf("T raw=%.2f avg=%.2f  |  P raw=%.2f ema=%.2f\n",
    t_raw, t_avg, p_raw, ema_p);
```
Nyers és szűrt értékek párhuzamos megjelenítése — így empirikusan megítélhető a szűrő minősége.

---

### 🚀 Kihívás

**1. szint:** Változtasd meg az `ALPHA_P` értékét 0.05-re és 0.8-ra. Vizsgáld a Serial outputon, hogyan változik az EMA reakcióideje. Tapintsd a szenzort és figyeld, mikor reagál a szűrt érték.

**2. szint:** Implementálj egy **medián-szűrőt** 5 mintából (rendezd az 5 legutóbbi értéket és válaszd a középsőt). Hasonlítsd az EMA-val: melyik szűr jobban ki egy hirtelen zajtüskét (spike)?

**3. szint — gondolkodós:** A Kalman-szűrő az EMA általánosítása: a simítási faktort (`α`-t) dinamikusan számítja a mérési és folyamatzaj modelljeből. Keress rá az 1D Kalman-szűrő diszkrét egyenletére — egyszerűbb-e implementálni, mint gondolnád?

---

### 🏆 Megszerzett jelvény

**🔬 Jelfeldolgozó Mérnöki Jelvény**
*EMA és SMA szűrők implementálva, paraméterezve és tesztelve. A szenzor nyers jelei zajszűrt formában kerülnek az állapotgépbe és a telemetriai csomagba.*

---

### ➡️ Következő fejezet

A 14. fejezetben összerakod az egészet: **teljes repülési szoftver** — állapotgép, EMA szűrők, többsebességű ütemező, SD, rádió, WiFi, tetőpont-számítás — egyetlen önálló rendszerben. Ez a CSÜA Küldetésvezető Tanúsítvány vizsgafeladata.
