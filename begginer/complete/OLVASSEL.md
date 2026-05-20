# CanSat Kezdő Keretrendszer

Ez a mappa tartalmaz mindent, amire szükséged van a CanSat programozásához.  
**Egyetlen fájlt kell szerkesztened:** `src/mission.cpp`  
**Egyetlen parancsot kell futtatnod:** `./cansat run`

---

## Első lépések

### 1. Telepítsd a PlatformIO-t

A PlatformIO fordítja és tölti fel a kódodat a CanSat-ra.

- **Letöltés:** https://platformio.org/install/cli
- Ellenőrzés: nyiss egy terminált és írd be: `pio --version`

### 2. Nyisd meg a projektet

Navigálj ebbe a mappába (`begginer/`) egy terminálban:

```
cd útvonal/cansat-edu/begginer
```

### 3. Szerkeszd a küldetésedet

Nyisd meg a `src/mission.cpp` fájlt egy szövegszerkesztőben (pl. VS Code).  
Ez az **egyetlen fájl**, amit módosítasz.

### 4. Töltsd fel és indítsd el

```
./cansat run
```

Ez automatikusan:
1. Lefordítja a kódod
2. Feltölti a CanSat-ra
3. Megnyitja a soros monitort (ahol látod az adatokat)

---

## Parancsok

| Parancs | Mit csinál |
|---------|-----------|
| `./cansat run` | Fordít, feltölt, majd megnyitja a monitort |
| `./cansat build` | Csak fordít (hibakereséshez, feltöltés nélkül) |
| `./cansat monitor` | Megnyitja a soros monitort |
| `./cansat new` | Visszaállítja a `mission.cpp` fájlt az eredeti sablonra |
| `./cansat help` | Kiírja a parancsokat |

**Windows-on** ugyanezek a parancsok, de `./cansat` helyett csak `cansat`:
```
cansat run
cansat build
```

---

## Mit írj a mission.cpp-be?

A kódodnak két részből kell állnia:

```cpp
#include <cansat.h>

// Egyszer fut le, bekapcsoláskor
void mission_setup() {
    // pl. WiFi csatlakozás
}

// Minden 2 másodpercben lefut
void mission_loop() {
    // ide írd a küldetésed logikáját
}
```

---

## Az érzékelők (sensors)

```cpp
float homerseklet  = sensors.temperature();  // Celsius fok
float nyomas       = sensors.pressure();     // hPa (légnyomás)
float paratartalom = sensors.humidity();     // százalék (%)

float gyorsulas_x = sensors.accel_x();  // g egységben
float gyorsulas_y = sensors.accel_y();
float gyorsulas_z = sensors.accel_z();

float forgassebesseg_x = sensors.gyro_x();  // fok/másodperc
float forgassebesseg_y = sensors.gyro_y();
float forgassebesseg_z = sensors.gyro_z();
```

**Tipp:** Hívd meg a `sensors.temperature()`-t először — az frissíti az összes többi értéket is.

---

## A rádió (radio)

LoRa rádión keresztül küldi el az adatokat a földi állomásnak.

```cpp
// Három számot küld (vesszővel elválasztva)
radio.send(homerseklet, nyomas, paratartalom);

// Vagy egy saját szöveget
radio.send("indulas!");
```

> **Fontos:** A fizikai kapcsolót állítsd **Radio** állásba repülés előtt, és **USB** állásba, ha a monitort akarod használni!

---

## A WiFi

```cpp
// Csatlakozás a hálózathoz (mission_setup()-ban hívd meg)
wifi.connect("HálózatNeve", "Jelszó");

// Adatküldés egy webcímre (pl. saját szerver vagy dashboard)
wifi.send("http://192.168.1.5/adat", homerseklet, nyomas, paratartalom);
```

---

## Az SD kártya (sd)

Az adatokat CSV fájlba menti az SD kártyán (minden indítás új mappát hoz létre: S001, S002...).

```cpp
// Sor mentése a telem.csv fájlba
sd.log(homerseklet, nyomas, paratartalom);

// Esemény mentése az events.txt fájlba (bármilyen szöveg)
sd.note("indulas!");
sd.note("maximum magassag elert");
```

---

## Teljes példa

```cpp
#include <cansat.h>

void mission_setup() {
    // WiFi nem kell? Hagyd üresen.
}

void mission_loop() {
    float t = sensors.temperature();
    float p = sensors.pressure();
    float h = sensors.humidity();

    // Kiírás a monitorra
    Serial.print("Hőmérséklet: "); Serial.print(t); Serial.println(" C");
    Serial.print("Légnyomás:   "); Serial.print(p); Serial.println(" hPa");

    // Küldés rádión
    radio.send(t, p, h);

    // Mentés SD kártyára
    sd.log(t, p, h);
    sd.note("mérés OK");
}
```

---

## Ha valami nem megy

| Hibaüzenet / probléma | Megoldás |
|-----------------------|---------|
| `pio: command not found` | Telepítsd a PlatformIO-t: https://platformio.org/install/cli |
| Fordítási hiba | Nézd meg a hibaüzenetet — általában egy elgépelt szó vagy hiányzó `;` |
| Feltöltés sikertelen | Ellenőrizd az USB kábelt; tartsd lenyomva a BOOT gombot, majd nyomd meg a RESET-et |
| Nem látok semmit a monitoron | Ellenőrizd, hogy a kapcsoló **USB** állásban van-e |
| Rádió nem küld | Ellenőrizd, hogy a kapcsoló **Radio** állásban van-e repülés közben |
| SD kártya nem működik | Ellenőrizd, hogy be van-e helyezve; FAT32 formátumra kell formázni |

---

## Hardver összefoglaló

| Alkatrész | Funkció |
|-----------|---------|
| ESP8266 (ESP-12E) | Az agy — ez futtatja a kódodat |
| BME280 | Hőmérséklet, légnyomás, páratartalom |
| LSM6DSOX | Gyorsulásmérő + giroszkóp |
| RN2483 | LoRa rádió adó-vevő |
| SD kártya | Adatnaplózás |

---

*Jó küldetést!*
