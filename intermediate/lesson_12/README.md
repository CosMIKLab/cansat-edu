# 🛰️ Mérnöki Napló — 12. fejezet: Valósidejű Időzítés
## CSÜA Műszaki Divízió · Ütemező Alrendszer

A rendszerteszten kritikus probléma derül ki: a `delay(2000)` használata blokkolja a teljes programot. Amíg a rádió csomag küldése zajlik (2 mp), az IMU nem mér — és pont ilyenkor lehetne felszállás.

*"Beágyazott rendszerekben a `delay()` tilalmas. Egy repülési számítógépnek párhuzamosan kell kezelnie több feladatot különböző sebességeken. Az RTOS (Real-Time Operating System) helyett egyszerű `millis()`-alapú ütemezőt implementálunk — ez elegendő a mi igényeinkhez."*

---

### 🎯 Tanulási célok

- `millis()` alapú nem-blokkoló időzítés mintázata
- Több függetlenül ütemezett feladat egyetlen `loop()`-ban
- Miért tilos a `delay()` valódi repülési szoftverben
- Időzítési pontosság és jitter analízis
- `constexpr` konstansok az időzítési értékekhez

---

### 💻 Kódmagyarázat

```cpp
static uint32_t tSensor = 0;
static constexpr uint32_t DT_SENSOR = 100;   // ms
```

Minden feladathoz egy **időzítő változó** és egy **intervallum konstans**. A `static` biztosítja, hogy az értékek a `loop()` hívások között megmaradjanak. A `constexpr` fordításidőben értékelődik ki — nincs RAM és futásidejű overhead.

```cpp
void loop() {
    uint32_t now = millis();

    if (now - tSensor >= DT_SENSOR) {
        tSensor = now;
        // szenzor olvasás...
    }
    if (now - tSD >= DT_SD) { ... }
    if (now - tRadio >= DT_RADIO) { ... }
}
```

**Miért `now - tSensor` és nem `now > tSensor + DT_SENSOR`?** Az előbbi helyesen kezeli a `millis()` 32-bites rollover-ét (49.7 nap után nullázódik). A különbség előjeltelenül számolódik — a rollover átlépésekor is helyes eredményt ad.

**Ütemezési diagram (egy másodpercre):**
```
t=0ms    : szenzor ✓
t=100ms  : szenzor ✓
t=200ms  : szenzor ✓
t=500ms  : szenzor ✓ + SD ✓
t=1000ms : szenzor ✓ + SD ✓ + serial ✓
t=2000ms : szenzor ✓ + SD ✓ + serial ✓ + rádió ✓
```

A `loop()` maga gyorsan ismétlődik (kb. 10 000×/másodperc), és minden hívásban csak ellenőriz — nem vár.

---

### 🚀 Kihívás

**1. szint:** Mérd meg a szenzor-olvasás tényleges időközét: tárolj egy `prevSensorTime` változót, és írd ki `now - prevSensorTime` értékét minden szenzor-olvasásnál. Mennyi a jitter (szórás az ideális 100 ms-hoz képest)?

**2. szint:** Adj hozzá egy 5. ütemezett feladatot: 10 másodpercenként hívj `Storage::event("HEARTBEAT")`-et. Ez hasznos az events.txt adatok utólagos elemzésekor.

**3. szint — gondolkodós:** A `millis()` visszatér 0-ra 49.7 nap után. Ha `tSensor = 49.99 nap` és `now = 0.01 nap` (rollover), akkor `now - tSensor` egy nagy pozitív szám, ami szinte azonnal triggerel. Miért helyes ez? Bizonyítsd be aritmetikailag, hogy az előjel nélküli kivonás helyesen kezeli a rollover-t.

---

### 🏆 Megszerzett jelvény

**⏱️ Valósidejű Ütemező Jelvény**
*A `delay()`-mentes többfeladatos ütemező implementálva. A rendszer 10 Hz-en mintavételez, miközben párhuzamosan kezeli az SD, rádió és WiFi feladatokat.*

---

### ➡️ Következő fejezet

A 13. fejezetben az eddig nyers szenzorjeleket **szűröd**: exponenciális mozgó átlag a nyomásra és az az-tengelyre, csúszó átlag a hőmérsékletre. Megtanulod az `ALPHA` paraméter empirikus hangolását.
