# 🛰️ Küldetési Napló — 10. fejezet: A teljes küldetés
## CSÜA Küldetésközpont · Indítás Napja

A küldetésközpont kivetítőin piros villogó felirat: **T-10 PERC AZ INDÍTÁSIG**.

A vezető odajön hozzád, és komolyan néz: *"Junior Irányító, ez az a nap, amiért az összes felkészülés folyt. Az összes rendszert tesztelted — szenzorok, SD kártya, rádió, WiFi, repülési logika. Most mindezt egybe kell rakni. Ez lesz a végleges küldetési kód."*

Visszagondolsz az elmúlt 9 leckére:
- Az 1.-ben megtanultad, hogyan kapcsol be a CanSat
- A 2-4.-ben aktiváltad az összes szenzort
- Az 5.-ben megtanítottad dönteni
- A 6.-ban fekete dobozt adtál neki
- A 7.-ben rádión küld adatokat
- A 8.-ban WiFi-re kapcsolódott
- A 9.-ben megtanult önállóan felismerni a repülési fázisokat

Most minden egyszerre megy. A CanSat készen áll.

---

### 🎯 Ebben a leckében:

- Összefoglalod az összes tanult elemet egyetlen programban
- Látod, hogyan működnek együtt a különböző rendszerek
- Megérted, hogy egy valódi CanSat küldetési kód pontosan így néz ki

---

### 💻 A teljes kód magyarázata

```cpp
bool felszallt      = false;
bool tetopont_elert = false;
float min_nyomas    = 1013.25;
```
Globális változók a repülési fázisok nyomon követéséhez (9. fejezet).

```cpp
void mission_setup() {
    wifi.connect("IskolaHalozat", "jelszo123");
    min_nyomas = sensors.pressure();
    sd.note("CSUA Kuldetes indítva");
    Serial.println("  CSÜA CanSat — KÉSZEN ÁLL  ");
}
```
Indításkor: WiFi csatlakozás (8. fejezet) + alap légnyomás rögzítése (9. fejezet) + esemény naplózás (6. fejezet).

```cpp
    sd.log(homerseklet, nyomas, paratartalom);     // 6. fejezet
    radio.send(homerseklet, nyomas, paratartalom); // 7. fejezet
    if (wifi.connected()) {                        // 8. fejezet
        wifi.send("http://192.168.1.5/telemetria", ...);
    }
```
Minden mérési ciklusban: SD mentés + rádió küldés + WiFi küldés (ha van kapcsolat).

```cpp
    if (!felszallt && az > 2.0) { ... }            // Felszállás (9. fejezet)
    if (felszallt && nyomas < min_nyomas) ...       // Tetőpont követés
    if (felszallt && !tetopont_elert && ...) { ... } // Tetőpont
```
Repülési esemény-érzékelők — az SD kártyára és rádión is jelzik az eseményeket.

```cpp
    Serial.print("T="); Serial.print(homerseklet);
    Serial.print("  P="); Serial.print(nyomas);
    Serial.print("  H="); Serial.print(paratartalom);
    Serial.print("  az="); Serial.println(az);
```
Tömör adatsor a monitoron — fejlesztés közben hasznos, repülés közben nem látja senki.

---

### 🚀 Kihívás — Saját küldetés tervezése

Most, hogy ismered az összes eszközt, tervezd meg a **saját küldetésedet**!

**1. kihívás:** Cseréld le a WiFi hálózat adatait és az URL-t a valódi iskola adataira.

**2. kihívás:** Adj hozzá egy saját tudományos mérést! Ötletek:
- Mérd a hőmérséklet változásának sebességét (mennyi fokot változik másodpercenként)
- Küldd el rádión a gyorsulásvektort is (`az`)
- Írj `sd.note`-ot, ha a páratartalom 70% fölé megy

**3. kihívás — saját küldetés:** Töröld ki az összes megjegyzést és módosítsd a kódot a **saját CanSat csapatod mérési céljaihoz**. Mi a ti tudományos kérdésetek? Milyen adatokra van szükségetek?

> **Tipp:** Bármikor vissza tudsz térni bármelyik korábbi leckéhez, ha elfelejtenél valamit. A `lesson_1` – `lesson_9` mappákban ott van minden magyarázat és példakód.

---

### 🏆 VÉGSŐ JELVÉNY

**🚀 Teljes Küldetés — Arany Jelvény**

*Teljesítetted a CSÜA Junior Küldetésirányítói Program összes fejezetét. Megírtad az első teljes CanSat küldetési kódot — szenzorokkal, SD kártyás naplózással, LoRa rádióval, WiFi telemetriával és önálló repülési logikával.*

*A CanSat készen áll az indításra. Jó küldetést!*

```
╔══════════════════════════════════╗
║   CSÜA — KÜLDETÉS ENGEDÉLYEZVE  ║
║   Junior Irányító: [A TE NEVED]  ║
║   T-0 — INDÍTÁS!                ║
╚══════════════════════════════════╝
```

---

### 📚 Mit tanultál a 10 fejezetben

| Fejezet | Mit tanultál |
|---------|-------------|
| 1 | `setup` / `loop`, `Serial.println` |
| 2 | `float` változó, `sensors.temperature()` |
| 3 | Több változó, légnyomás, páratartalom |
| 4 | Gyorsulásmérő, g egység, 3 tengely |
| 5 | `if` / `else`, összehasonlítások |
| 6 | SD kártya, CSV, `sd.log()`, `sd.note()` |
| 7 | LoRa rádió, `radio.send()`, duty cycle |
| 8 | WiFi, HTTP GET, `wifi.connect()`, `wifi.send()` |
| 9 | `bool`, állapotgép, felszállás/tetőpont érzékelés |
| 10 | Teljes küldetési kód — minden egyszerre |
