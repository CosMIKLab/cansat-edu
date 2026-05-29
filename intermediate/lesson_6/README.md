# 🛰️ Mérnöki Napló — 6. fejezet: Adattárolás
## CSÜA Műszaki Divízió · Fedélzeti Adatarchívum

A Rendszertervező egy újabb követelményt közöl: *"A LoRa kapcsolat instabil lehet felhős időben. Minden telemetriai adatot SD kártyára is kell menteni — ez a fedélzeti 'fekete doboz'. A mentés FAT32 formátumban, CSV fájlba történik, automatikus munkamenet-kezeléssel. Minden indítás új mappát kap (S001, S002...) — soha ne írjuk felül a korábbi adatokat."*

Megnézed a `Storage` namespace API-ját. Három függvény: `init`, `log`, `event`. Egyszerű — de a `TelemetryRecord` struktúrát pontosan kell kitölteni.

---

### 🎯 Tanulási célok

- A `Storage` névtér API-ja (`init`, `log`, `event`, `flush`, `close`)
- A `TelemetryRecord` struktúra összes mezője
- `namespace` (névtér) fogalma C++-ban vs. osztály
- Nem-blokkoló mintázat: `if (millis() - lastLog < 2000) return;`
- Hibakezelés `init()` visszatérési értékén keresztül

---

### 💻 Kódmagyarázat

```cpp
if (!Storage::init(PIN_SD_CS)) {
    Serial.println("FIGYELEM: SD kártya nem elérhető");
}
```

A `Storage::init()` argumentuma az SPI chip-select pin (GPIO15). Ha az SD kártya nincs behelyezve vagy hibás, `false`-t ad vissza. A kód folytatódik SD nélkül — a `Storage::log()` belsőleg ellenőrzi az állapotot és csendben kihagyja a hívást.

```cpp
sensors_event_t accel, gyro, temp_e;
imu.getEvent(&accel, &gyro, &temp_e);

TelemetryRecord rec = {};
rec.time_ms  = millis();
rec.temp     = bme.readTemperature();
rec.pressure = bme.readPressure() / 100.0F;
rec.humidity = bme.readHumidity();
rec.ax = accel.acceleration.x / 9.80665f;
rec.ay = accel.acceleration.y / 9.80665f;
rec.az = accel.acceleration.z / 9.80665f;
rec.gx = gyro.gyro.x * (180.0f / M_PI);
rec.gy = gyro.gyro.y * (180.0f / M_PI);
rec.gz = gyro.gyro.z * (180.0f / M_PI);
```

A `= {}` inicializáció az összes mezőt 0-ra állítja. Fontos: soha ne hagyj inicializálatlan `float` mezőket — azok véletlenszerű értéket tartalmazhatnak.

```cpp
Storage::log(rec);
```

Ez beírja a `telem.csv` fájlba a rekordot a következő formátumban:
```
time_ms,temp_c,press_hpa,hum_pct,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps
```
Minden `SD_FLUSH_EVERY` (5) rekordnál a rendszer lemezre flusholja az adatokat — ez egyensúlyt teremt az SD élettartam és az adatvesztés kockázata között.

```cpp
Storage::event("BOOT_OK");
```
Az `events.txt` fájlba ír egy időbélyeges bejegyzést: `[   1234] BOOT_OK`. Ezt fontos események jelzésére használjuk (felszállás, tetőpont, landolás).

**C++ névtér (`namespace`) vs. osztály:** A `Storage` névtér nem példányosítható — nincs `Storage s;` szintaxis. A függvények statikusak a névtéren belül. Ez akkor megfelelő, ha csak egyetlen példányt akarunk (singleton minta névtérrel).

---

### 🚀 Kihívás

**1. szint:** A `setup()` végén hívd meg a `Storage::event("SZENZOR_OK")` függvényt, majd nézd meg az SD kártyán az `events.txt` tartalmát. Azonosítsd a munkamenet könyvtárát (S001, S002...).

**2. szint:** Implementálj egy egyszerű „adatminőség-ellenőrzőt": ha a mért nyomás értéke a fizikailag érvényes tartományon kívül esik (pl. `rec.pressure < 800.0f || rec.pressure > 1100.0f`), ne naplózd a rekordot és írd `Storage::event("READ_ERROR")`-t.

**3. szint — gondolkodós:** A CSV formátum emberi olvasásra szánt szöveges formátum. Milyen alternatívák léteznek binárisan (pl. Protocol Buffers, MessagePack, raw struct dump)? Melyiknek mi az előnye korlátozott tárterületen?

---

### 🏆 Megszerzett jelvény

**💾 Adatarchivum Jelvény**
*A fedélzeti adatlogger aktív. CSV fájl generálva, eseménynapló vezetve. A repülési misszió utólagos adatelemzése lehetővé vált.*

---

### ➡️ Következő fejezet

A 7. fejezetben az adatok **elhagyják a CanSat-ot**: a LoRa rádión keresztül valós időben jutnak el a földi állomáshoz. Megtanulod a bináris payload felépítését, a hex kódolást és a `Radio::send()` hívást.
