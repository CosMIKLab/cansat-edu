# 🛰️ Mérnöki Napló — 11. fejezet: Tetőpont-detektálás
## CSÜA Műszaki Divízió · Apogee Analitika

A szimulációs adatokon egy probléma látható: a nyomásjel természetes zajossága miatt a detektor egyszer-egyszer hamis tetőpontot jelez még az emelkedés közben. A Rendszertervező egy fehér táblán rajzol.

*"A nyers nyomásjel zajossága ±0.3–0.5 hPa. Ha csak a nyers minimumot figyeled, egy pillanatnyi zajcsúcs már tetőpontnak látszik. Két megoldás: EMA szűrő a bemenetre, és hisztérezis a kimeneten. Együtt a rendszer megbízhatóan detektál."*

Előveszed az EMA szűrő kódot.

---

### 🎯 Tanulási célok

- Exponenciális mozgó átlag (EMA) implementálása és alpha paraméter tuningolása
- Hisztérezis fogalma és alkalmazása zajszűrésre
- Nyomásminimum követés ASCENT fázisban
- Magasság kiszámítása a mért tetőpont-nyomásból
- `powf()` függvény a barometrikus formulában

---

### 💻 Kódmagyarázat

```cpp
static constexpr float EMA_ALPHA = 0.3f;
static float ema_pressure = 0.0f;

// Minden ciklusban:
ema_pressure = EMA_ALPHA * p + (1.0f - EMA_ALPHA) * ema_pressure;
```

Az **EMA (Exponential Moving Average)** szűrő egyetlen sor kód:
```
EMA_új = α × mérés + (1−α) × EMA_régi
```

- **Nagy alpha (0.7–0.9):** A szűrő gyorsan követi az újabb értékeket — kevéssé szűr, gyors reakció
- **Kis alpha (0.1–0.3):** Erősen simít, lassabb reakció — de jobban szűri a zajt
- **alpha = 0.3** jó kompromisszum: ~1 másodperces idő-konstans 500 ms mintavételezésnél

```cpp
if (ema_pressure < minPressure) minPressure = ema_pressure;
```
A minimum követő minden ciklusban frissül, ha az EMA értéke csökkent. A `9999.0f` kezdeti érték garantálja, hogy az első valódi mérés azonnal frissíti.

```cpp
if (ema_pressure > minPressure + 3.0f) {
    // tetőpont megerősítve
}
```

A **3 hPa hisztérezis** jelenti: a detektor csak akkor jel, ha a szűrt nyomás a minimumnál 3 hPa-val magasabb. Ez lehetővé teszi, hogy a zaj ne indítson hamis tetőpontot — a ±0.5 hPa zajtól 6× akkora a hisztérezis sávja.

```cpp
float alt = 44330.0f * (1.0f - powf(minPressure / groundPressure, 0.1903f));
Serial.printf("TETŐPONT: %.1f m\n", alt);
```

A minimális nyomásból visszaszámolja a tetőpont magasságát — ez a legpontosabb becslés.

---

### 🚀 Kihívás

**1. szint:** Kísérletezz az `EMA_ALPHA` értékével. Állítsd 0.1-re és 0.9-re, majd nézd a Serialon, hogyan változik a szűrt értékek reakciókészsége. Melyik értéknél tűnik el a zaj leginkább?

**2. szint:** Implementálj egy **derivált detektort**: ha az EMA nyomás 3 egymást követő mintában monoton csökken, tekintsd az emelkedést biztosnak. Ehhez tárold az előző 3 EMA értéket és hasonlítsd össze.

**3. szint — gondolkodós:** Az EMA szűrő egy IIR (Infinite Impulse Response) szűrő. Miért IIR? Mi a különbség az FIR (Finite Impulse Response) szűrőhöz képest? Melyiket alkalmaznád, ha a lekésés (latency) kritikus, és melyiket, ha a memóriakorlát a kötöttség?

---

### 🏆 Megszerzett jelvény

**📈 Tetőpont-analitikus Jelvény**
*Az EMA szűrővel és 3 hPa hisztérezissel védett apogee-detektor bevetésre kész. A hamis detekciók valószínűsége a zajszint alapján <1%.*

---

### ➡️ Következő fejezet

A 12. fejezetben az eddig `delay()`-alapú kódot **valódi többsebességű ütemezővé** alakítod: szenzorokat 10 Hz-en, SD-t 2 Hz-en, rádiót 0.5 Hz-en, monitort 1 Hz-en futtatva — egyetlen `delay()` sem lesz a `loop()`-ban.
