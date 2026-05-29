# 🛰️ Mérnöki Napló — 9. fejezet: Állapotgép
## CSÜA Műszaki Divízió · Repülési Logika

A repülési szoftver specifikációja megérkezik. *"A CanSat nem hagyhatja ugyanazt a kódot futni felszállástól landolásig. A különböző fázisokban más-más logikára van szükség: várás, felszállás detektálás, tetőpont keresés, ereszkedés követés, landolás megerősítés. Ezt állapotgéppel kezeljük."*

Az állapotgép a beágyazott rendszerek alapmintázata. A kódod mostantól „tudja, hol tart".

---

### 🎯 Tanulási célok

- `enum class` típus — erősen típusos állapotok
- `switch / case` utasítás — állapot-alapú elágazás
- Állapotátmenetek és a triggert kiváltó feltételek
- Esemény-naplózás állapotváltáskor
- Miért jobb az állapotgép mint sok egymásba ágyazott `if`?

---

### 💻 Kódmagyarázat

```cpp
enum class FlightPhase { INIT, READY, ASCENT, APOGEE, DESCENT, LANDED };
static FlightPhase phase = FlightPhase::INIT;
```

Az `enum class` **erősen típusos** felsorolás — a `FlightPhase::ASCENT` nem konvertálható automatikusan `int`-té, így véletlen értékadási hibák nem fordulhatnak elő. A `static` biztosítja, hogy a változó megmarad az egész program életciklusa alatt.

```cpp
const char* phaseName(FlightPhase p) {
    switch (p) {
        case FlightPhase::ASCENT: return "ASCENT";
        // ...
    }
}
```

A `switch / case` az állapotgép szíve. Minden `case` ág egy állapothoz tartozó logikát futtat. A `break` (vagy `return`) megakadályozza, hogy a végrehajtás a következő ágba „essen".

```cpp
switch (phase) {
    case FlightPhase::READY:
        if (d.az > 2.0f) {
            phase = FlightPhase::ASCENT;
            Storage::event("ASCENT");
        }
        break;

    case FlightPhase::ASCENT:
        if (rec.pressure < minPressure) minPressure = rec.pressure;
        if (rec.pressure > minPressure + 2.0f) {
            phase = FlightPhase::APOGEE;
            Storage::event("APOGEE");
        }
        break;
```

**Állapotátmeneti logika:**
| Aktuális fázis | Feltétel | Következő fázis |
|---|---|---|
| READY | `az > 2.0g` | ASCENT |
| ASCENT | nyomás min + 2 hPa | APOGEE |
| APOGEE | automatikusan | DESCENT |
| DESCENT | nyomás ≈ talaj, az < 1.2g | LANDED |

---

### 🚀 Kihívás

**1. szint:** Add hozzá az állapot kiírásához az aktuális magasságot is (az előző fejezetből vett barometrikus formula). Jelenik-e meg a tetőpont magassága az APOGEE fázisban?

**2. szint:** Implementálj egy **időkorlátozást**: ha az ASCENT fázisban 60 másodperc elteltével nem észleltünk tetőpontot, lépj automatikusan DESCENT fázisba (timeout). Ehhez kell egy `ascentStart_ms` időbélyeg.

**3. szint — gondolkodós:** Az állapotgépek megvalósíthatók táblázatként is (transition table). Rajzolj egy állapotdiagramot (UML State Diagram) a jelenlegi fázisokhoz és átmenetekhez. Mit kellene módosítani, ha hozzáadnál egy EMERGENCY fázist, ahonnan bármelyik állapotból lehet átlépni?

---

### 🏆 Megszerzett jelvény

**🔄 Állapotgép Tervező Jelvény**
*A repülési állapotgép implementálva és tesztelve. A CanSat önállóan kezeli a READY → ASCENT → APOGEE → DESCENT → LANDED átmeneteket.*

---

### ➡️ Következő fejezet

A 10. fejezetben a felszállás detektálást **robusztussá** teszed: nem elég egy IMU spike — nyomástrendet is vizsgálsz. Megismered a gyűrűpuffer mintát és a kétfeltételes döntési logikát.
