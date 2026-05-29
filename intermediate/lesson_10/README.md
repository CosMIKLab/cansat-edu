# 🛰️ Mérnöki Napló — 10. fejezet: Felszállás-detektálás
## CSÜA Műszaki Divízió · Esemény-trigger Rendszer

A tesztelési eredmények aggasztóak: az előző verziójú detektor egyszer hamis riasztást adott, amikor valaki megütötte az asztalt. A Rendszertervező rendkívüli megbeszélést hív össze.

*"Az egyszerű IMU küszöb-detektor nem elég megbízható. Zajt, ütést, vibrációt egyaránt érzékel. Megbízható detekcióhoz két független feltételnek kell egyidejűleg teljesülnie: IMU gyorsulás-csúcs ÉS nyomástrend — együtt statisztikailag lehetetlen hamis riasztást adni."*

Előveszed a gyűrűpuffer kódot.

---

### 🎯 Tanulási célok

- **Gyűrűpuffer** (ring buffer / circular buffer) implementálása
- Nyomástrend detektálás mozgó átlaggal
- Kétfeltételes döntési logika (AND-gate trigger)
- `%` (modulo) operátor ciklikus indexeléshez
- Hamis pozitívok és hamis negatívok a detektálásban

---

### 💻 Kódmagyarázat

```cpp
static float pressureHistory[5] = {};
static uint8_t histIdx = 0;

// frissítés minden ciklusban:
pressureHistory[histIdx] = p;
histIdx = (histIdx + 1) % 5;
```

A **gyűrűpuffer** (circular buffer) 5 legutóbbi nyomásértéket tárol. A `% 5` modulo operátor az indexet ciklikusan tartja `0..4` között: `0→1→2→3→4→0→1→...` Így mindig csak a legutolsó 5 mérés van memóriában, régi értékek felülíródnak.

```cpp
bool isPressureFalling() {
    float sum = 0.0f;
    for (int i = 0; i < 5; i++) sum += pressureHistory[i];
    return (sum / 5.0f) < (groundPressure - 1.5f);
}
```

Az 5 minta átlaga ha 1.5 hPa-val alacsonyabb a talaj-referenciánál, a nyomás szisztematikusan esik — ez valódi emelkedést jelez.

```cpp
if (imuSpike && pressureFalling) {
    launched = true;
    Storage::event("LAUNCH_CONFIRMED");
}
```

**AND-gate logika:** Csak akkor jel, ha mindkét feltétel egyszerre teljesül. Ütés esetén: `imuSpike=true`, de `pressureFalling=false` → nincs riasztás. Lassú emelkedő légmozgás esetén: `pressureFalling=true`, de `imuSpike=false` → nincs riasztás.

**Küszöbértékek tervezési szempontjai:**
- `az > 2.5g` — a rakétaindítás 5–20g csúcsot produkál; 2.5g elég érzékeny de zajra nem vált ki
- `1.5 hPa` csökkentés — ~12 méter emelkedésnek felel meg, stabil körülmények között

---

### 🚀 Kihívás

**1. szint:** Növeld a gyűrűpuffer méretét 10-re. Hogyan változik a detekció reagálási sebessége? Mi az átlagolás „ára" (késés)?

**2. szint:** Adj hozzá egy **debounce** mechanizmust: miután az IMU spike-ot észlelted, 2 másodpercig ne vizsgáld újra — ezalatt ellenőrzöd a nyomástrendet. Ehhez kell egy `spikeTime_ms` időbélyeg.

**3. szint — gondolkodós:** A Bayes-döntési elméletben a P(felszállás | az>2.5g ÉS dP>1.5hPa) meghatározható prior valószínűségekkel. Becsüld meg: mekkora a valószínűsége, hogy asztalütés esetén mindkét feltétel egyszerre teljesül? Hogyan csökkenti ez a hamis pozitívok arányát?

---

### 🏆 Megszerzett jelvény

**🚀 Felszállás-detektáló Jelvény**
*A kétfeltételes felszállás-detektor implementálva: IMU spike + nyomástrend kombinációja. A hamis riasztások valószínűsége <0.1%.*

---

### ➡️ Következő fejezet

A 11. fejezetben a **tetőpont detektálást** implementálod robusztus módon: EMA nyomásszűrővel és hisztérezis-logikával. Megérted, miért okoz problémát a nyers nyomásjel és hogyan oldja meg a szűrés.
