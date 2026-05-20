# 🛰️ Küldetési Napló — 9. fejezet: Repülési logika
## CSÜA Küldetésközpont · Küldetés-intelligencia

*"Holnap a próbarepülés"* — jelenti be a küldetésvezető. *"De előtte az utolsó nagy teszt: a CanSat-nak önállóan kell felismernie, hogy éppen melyik fázisban van a repülés."*

A missziós profil három fázisból áll:
1. **Felszállás** — a rakéta magával viszi a CanSat-ot, nagy gyorsulás
2. **Tetőpont** — a maximális magasság elérése, a gyorsulás megszűnik
3. **Ereszkedés** — az ejtőernyő kinyílik, lassan süllyedés

A CanSat-nak mindezt automatikusan kell észlelnie — és az eseményeket az SD kártyán naplózni. Ez a szoftvermérnökség egyik alapelemének, az **állapotgépnek** az első megvalósítása.

---

### 🎯 Ebben a leckében megtanulod:

- Mi az a **`bool`** változó (igen/nem értéket tárol)
- Hogyan emlékezzen a program arra, mi történt korábban
- Hogyan detektálj **felszállást** (nagy gyorsulás) és **tetőpontot** (légnyomás visszaemelkedés)
- Mi az egy egyszerű **állapotgép**

---

### 💻 A kódod magyarázata

```cpp
bool felszallt      = false;
bool tetopont_elert = false;
float min_nyomas    = 1013.25;
```

Ezek a változók a `mission_setup` és `mission_loop` **felett** vannak — azért, hogy **mindkét függvény látja és módosítani tudja** őket. `false` = nem történt meg, `true` = már megtörtént.

**`bool`** — logikai típus, csak `true` (igaz) vagy `false` (hamis) értéket vehet fel.

```cpp
void mission_setup() {
    min_nyomas = sensors.pressure();
    ...
}
```
Induláskor feljegyezzük az aktuális légnyomást — ez lesz a "viszonyítási alap" a magasság méréséhez.

```cpp
    if (!felszallt && az > 2.0) {
        felszallt = true;
        sd.note("FELSZALLAS ESZLELVE");
        ...
    }
```
`!felszallt` — az `!` jel azt jelenti: "NEM" — tehát "ha MÉG NEM szállt fel". Ha a Z-gyorsulás meghaladja 2g-t, nagy valószínűséggel a rakéta gyorsít — felszállás!

```cpp
    if (felszallt && nyomas < min_nyomas) min_nyomas = nyomas;
```
Folyamatosan nyomon követjük a legalacsonyabb légnyomást — ez a legnagyobb magassághoz tartozik.

```cpp
    if (felszallt && !tetopont_elert && nyomas > min_nyomas + 2.0) {
        tetopont_elert = true;
        sd.note("TETOPONT ELERT");
        ...
    }
```
Ha már felszállt, de a tetőpont még nem volt, és a légnyomás már 2 hPa-val visszaemelkedett a minimumhoz képest — megtaláltuk a tetőpontot! (A légnyomás magasabban kisebb, lentebb nagyobb.)

---

### 🚀 Kihívás

**1. szintű kihívás:** Add hozzá a **leszállás** érzékelését is! Az ejtőernyős leszállás után a légnyomás visszaáll az indulási értékhez közel. Próbáld meg:
```cpp
bool leszallt = false;
// ...
if (tetopont_elert && !leszallt && nyomas > min_nyomas + 20.0) {
    leszallt = true;
    sd.note("LESZALLAS");
    Serial.println("*** LESZÁLLÁS! ***");
}
```

**2. szintű kihívás:** A felszállás érzékelés jelenleg `az > 2.0`-t figyel. De mi van, ha csak megrázzák a CanSat-ot és az is 2g fölé megy? Hogyan lehetne megbízhatóbbá tenni? *(Gondolj bele: egyetlen mérés, vagy több egymás utáni mérés kéne?)*

**3. szintű kihívás — gondolkodós:** A `felszallt`, `tetopont_elert` változókat miért nem lehet a `mission_loop`-on belül létrehozni? Mi történne, ha ott hoznád létre?

---

### 🏆 Megszerzett jelvény

**🎯 Küldetés-Intelligencia Jelvény**
*A CanSat önállóan képes felismerni és naplózni a repülési fázisokat. Az állapotgép aktív. A küldetésirányítás megerősítette: a rendszer repülésre kész.*

---

### ➡️ Következő fejezet

Az **utolsó fejezet** — az **Indítás Napja**! Összegyűjtöd mindazt, amit az összes korábbi leckéből tanultál, és megírod a **teljes küldetési kódot**: szenzorok, SD kártya, rádió, WiFi, repülési logika — minden egyszerre, egyetlen programban. Ez az igazi CanSat küldetés.
