# 🛰️ Küldetési Napló — 6. fejezet: Adatnaplózás
## CSÜA Küldetésközpont · Repülési Felvevő

A tudományos vezető összehívja a csapatot: *"Eddig minden mérési adat csak a monitoron jelent meg — és el is tűnt. Repülés közben nem lesz USB kábel, nem lesz monitor. Az adatokat el kell menteni a CanSat fedélzetén, hogy leszállás után vissza tudjuk tölteni és elemezni tudjuk."*

Felmutat egy apró, bélyegnyi méretű kártyát: *"Ez az SD kártya lesz a CanSat 'fekete doboza'. Pont mint a repülőgépeken — még ha minden más meghibásodik, a felvevő megőrzi az adatokat."*

A feladatod: programozd be az SD kártyás adatnaplózást. Minden mérési ciklus adatai kerüljenek egy CSV fájlba, a fontos események pedig egy eseménynaplóba.

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan mentsd az adatokat SD kártyára (`sd.log()`)
- Hogyan jelölj meg eseményeket szöveggel (`sd.note()`)
- Mi az a **CSV fájl**, és hogyan nyithatod meg Excelben
- Hogyan hozza létre a CanSat automatikusan az új munkamenet-mappákat

---

### 💻 A kódod magyarázata

```cpp
void mission_setup() {
    Serial.println("SD kártya adatnaplózás teszt...");
    sd.note("Rendszer elindult!");
}
```

`sd.note("Rendszer elindult!")` — egy szöveges bejegyzést ír az `events.txt` fájlba. Az indítási eseményt érdemes itt rögzíteni, mert a `mission_setup` egyszer fut le.

```cpp
void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    sd.log(homerseklet, nyomas, paratartalom);
    sd.note("Meres elvegezve");
    ...
}
```

`sd.log(a, b, c)` — egy sort ír a `telem.csv` fájlba. Automatikusan hozzáadja az időbélyeget (ms). A sor így néz ki a fájlban:
```
time_ms,temp_c,press_hpa,hum_pct,...
2000,23.45,1013.25,48.70,...
```

`sd.note("szöveg")` — egy sort ír az `events.txt` fájlba, időbélyeggel:
```
[    2000] Rendszer elindult!
[    4312] Meres elvegezve
```

**Munkamenet-mappák:** Minden bekapcsoláskor a CanSat egy új mappát hoz létre az SD kártyán: `S001`, `S002`, stb. Így az adatok soha nem keverednek össze a különböző repülésekből.

**Az SD kártyát ki kell venni** és számítógépbe dugni az adatok letöltéséhez. FAT32 formátumra kell formatálva lennie.

> **Figyelem:** Az `sd.note`-ban érdemes elkerülni az ékezeteket, mert a FAT32 fájlrendszer nem mindig kezeli jól őket: `"Meres elvegezve"` jó, `"Mérés elvégezve"` problémás lehet.

---

### 🚀 Kihívás

**1. szintű kihívás:** Távolítsd el az `sd.note("Meres elvegezve")` sort a `mission_loop`-ból — ez minden 2 másodpercben ír, ami hamar megtelik az events.txt. Az `sd.note`-ot csak fontos eseményekre érdemes használni!

**2. szintű kihívás:** Adj hozzá egy dőlés-érzékelést: ha `az > 1.5`, írj egy `sd.note("Megdolt!")` bejegyzést. Döntsd meg a CanSat-ot, majd vedd ki az SD kártyát és nézd meg az `events.txt` tartalmát!

**3. szintű kihívás — gondolkodós:** Nyisd meg a `telem.csv` fájlt Excelben vagy Google Sheets-ben! Milyen grafikont tudnál belőle csinálni? Mi lenne az X tengely, mi lenne az Y?

---

### 🏆 Megszerzett jelvény

**💾 Repülési Felvevő Jelvény**
*Az SD kártya adatnaplózó rendszer aktív. A CanSat minden mérési ciklust rögzít, az eseményeket időbélyeggel naplózza. A fekete doboz készen áll a repülésre.*

---

### ➡️ Következő fejezet

A 7. fejezetben az adatok már **repülés közben**, valós időben jutnak el a földi állomásra: aktiváljuk a **LoRa rádiót**. Megtanulod, hogyan kódolódnak az adatok rádióhullámokká, és miért kell figyelni a 2 másodperces késleltetésre.
