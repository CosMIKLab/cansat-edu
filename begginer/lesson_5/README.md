# 🛰️ Küldetési Napló — 5. fejezet: Döntési logika
## CSÜA Küldetésközpont · Intelligens Figyelő

A küldetésirányítási osztály részéről egy üzenet érkezik: *"A CanSat nem lehet csak egy egyszerű adatgyűjtő — szüksége van saját intelligenciára. Ha valami nem stimmel, azonnal jelezze, ne csak adatokat gyűjtsön."*

A vezető mérnök elmagyarázza: *"Képzeld el, hogy a CanSat 40 km magasságban kering, és mi itt a földön nem tudjuk percenként figyelni az összes értéket. A műholdnak önállóan kell észlelni, ha valami rendkívüli történik — és figyelmeztetni minket. Ezt úgy érjük el, hogy feltételeket írunk a kódba."*

Ez a programozás egyik legalapvetőbb és legfontosabb eszköze: az **`if` / `else`** — vagyis 'ha ... akkor ... különben'. A CanSat eztán nemcsak mér, hanem **dönt** is.

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan működik az `if` / `else` feltétel
- Hogyan hasonlíts össze számokat (`>`, `<`, `==`)
- Hogyan detektálj rendkívüli eseményeket szenzor értékek alapján

---

### 💻 A kódod magyarázata

```cpp
void mission_loop() {
    float homerseklet = sensors.temperature();
    float az          = sensors.accel_z();
    ...
}
```
Két értéket olvasunk: hőmérséklet és Z-gyorsulás (dőlés érzékeléshez).

```cpp
    if (homerseklet > 30.0) {
        Serial.println("FIGYELEM: Magas hőmérséklet!");
    } else {
        Serial.print("Hőmérséklet normális: ");
        Serial.print(homerseklet);
        Serial.println(" °C");
    }
```

**Hogyan olvasd ezt?**
- `if (homerseklet > 30.0)` — **HA** a hőmérséklet nagyobb mint 30
- `{ ... }` — **AKKOR** futtasd a zárójelben lévő kódot
- `else { ... }` — **KÜLÖNBEN** futtasd ezt

Az összehasonlítás operátorai:
| Jel | Jelentés |
|-----|---------|
| `>` | nagyobb mint |
| `<` | kisebb mint |
| `>=` | nagyobb vagy egyenlő |
| `<=` | kisebb vagy egyenlő |
| `==` | egyenlő (két egyenlőjel!) |
| `!=` | nem egyenlő |

```cpp
    if (az < 0.5) {
        Serial.println("FIGYELEM: A CanSat erősen megdőlt!");
    } else {
        Serial.println("Helyzet: normális");
    }
```

Ha `az` (Z-tengely gyorsulás) kisebb mint 0.5 g, a CanSat kb. 60 foknál jobban dől — ez repülés közben azt jelenti, hogy valami nincs rendben az orientációval.

---

### 🚀 Kihívás

**1. szintű kihívás:** Add hozzá a páratartalom ellenőrzését is:
```cpp
float paratartalom = sensors.humidity();
if (paratartalom > 80.0) {
    Serial.println("FIGYELEM: Magas páratartalom — köd lehetséges!");
} else {
    Serial.print("Páratartalom: ");
    Serial.print(paratartalom);
    Serial.println(" %");
}
```

**2. szintű kihívás:** Módosítsd a hőmérsékleti határt — ne 30°C, hanem a **jelenlegi szobahőmérséklet + 2°C** legyen a határ. Nézd meg a monitoron, hogy mi az aktuális hőmérséklet, majd állítsd be a határt. Most a FIGYELEM-nek meg kell jelennie, ha odatartod a kezed a szenzor közelé!

**3. szintű kihívás — gondolkodós:** Hogyan írnád le ezt a feltételt: "Ha a hőmérséklet 0°C alatt VAN, ÉS a páratartalom 90% felett van, akkor írja ki: 'Jegesedés veszélye!'"? *(Ehhez a `&&` (és) operátor kell: `if (a < 0 && b > 90)`)*

---

### 🏆 Megszerzett jelvény

**🧠 Intelligencia-Modul Jelvény**
*A CanSat fedélzeti döntési rendszere aktív. A műhold önállóan képes észlelni rendkívüli állapotokat és figyelmeztetést küldeni. A küldetésirányítási osztály elégedett.*

---

### ➡️ Következő fejezet

A 6. fejezetben az adatok nem csak a monitoron tűnnek el — **elmentjük őket!** Az SD kártya az igazi "fekete doboz": mindent rögzít, amit a CanSat mér, még akkor is, ha a rádió nem működik. Megtanulod, hogyan készíts CSV fájlt és hogyan jelöld meg a fontos eseményeket.
