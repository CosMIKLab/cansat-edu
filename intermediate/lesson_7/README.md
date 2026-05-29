# 🛰️ Mérnöki Napló — 7. fejezet: Rádió kommunikáció
## CSÜA Műszaki Divízió · LoRa RF Alrendszer

A kommunikációs osztályról megérkezik a rádiófrekvenciás engedélyezési dokumentum. *"A CanSat RN2483 chipje az ISM 868 MHz-es sávban sugároz, 12 dBm adóteljesítménnyel, SF7 spreading factor-ral. A payload maximuma 242 byte. A ti feladatotok: tömör bináris csomagot építeni, amely tartalmazza a legfontosabb telemetriai adatokat — minél kisebb méretben."*

Megnyitod a `radio.hpp` fájlt. Egyetlen `send()` függvény, nyers bájt-tömbbel.

---

### 🎯 Tanulási célok

- `Radio::send()` API — nyers byte tömb, hex kódolás
- Bináris payload tervezés (int16_t, int8_t, uint8_t használata)
- Szekvencia szám (sequence number) szerepe
- LoRa alapfogalmak: SF, frekvencia, adóteljesítmény
- Bitwisw operátorok: `>>`, `&` (byte kinyerés)

---

### 💻 Kódmagyarázat

```cpp
if (!radio.init()) { Serial.println("HIBA: Rádió"); while (1) delay(10); }
```
Az `init()` 5 AT-parancsot küld az RN2483 modulnak a soros porton: `mod lora`, `freq 868100000`, `sf sf7`, `pa off`, `pwr 12`. Ha bármelyik nem sikerül, `false`-t ad vissza.

```cpp
int16_t t_raw = (int16_t)(rec.temp * 100.0f);
int16_t p_raw = (int16_t)(rec.pressure * 10.0f);
```

Lebegőpontos értéket egésszé alakítunk — ez drasztikusan csökkenti a payload méretét:
- `23.45°C × 100 = 2345` → 2 byte (`int16_t`) — 0.01°C felbontás
- `1013.25 hPa × 10 = 10132` → 2 byte — 0.1 hPa felbontás
- `float` (4 byte) helyett 2 byte: 50% megtakarítás mezőnként

```cpp
uint8_t payload[9];
payload[0] = t_raw >> 8;    payload[1] = t_raw & 0xFF;
payload[2] = p_raw >> 8;    payload[3] = p_raw & 0xFF;
```

Az `int16_t` két byte-ból áll: MSB (most significant byte) és LSB (least significant byte). A `>> 8` a felső byte-ot hozza le, az `& 0xFF` a alsó byte-ot maszkolja ki. Ez a **big-endian** sorrendű byte-szétbontás.

```cpp
payload[5] = (int8_t)(rec.ax * 100.0f);
```
Az `int8_t` −128 ... +127 tartományú. Gyorsulás ×100: `2.0g → 200` — ez túl nagy! Ezért a gyorsulást csak durva felbontással (pl. 0.01g) tárolhatjuk így. Pontosabb megoldás: `int16_t` külön.

```cpp
payload[8] = txSeq++;
```
A szekvencia szám minden csomagban eggyel nő. A vevő oldalon így észlelhető, ha egy csomag kiesett (pl. `seq = 5, 7 → 6-os elveszett`).

---

### 🚀 Kihívás

**1. szint:** Számítsd ki, hány byte lenne a payload, ha minden értéket `float`-ként küldenél (6 érték + szekvencia). Mennyit takarítottál meg a jelenlegi 9-byte megoldással?

**2. szint:** Bővítsd a payloadot az aktuális FlightPhase állapottal (1 byte enum értékként). Ehhez definiálj egy egyszerű `enum Phase { READY=0, ASCENT=1, DESCENT=2, LANDED=3 };` típust és add hozzá a tömb végéhez.

**3. szint — gondolkodós:** A LoRa SF7 körülbelül 5 km hatótávolságot biztosít nyílt terepen. Mi az összefüggés a spreading factor, az adatsebesség és a hatótávolság között? Mikor lenne érdemes SF9-et vagy SF12-t használni?

---

### 🏆 Megszerzett jelvény

**📻 RF Kommunikációs Jelvény**
*A LoRa rádió alrendszer konfigurálva és tesztelve. Bináris telemetriai csomag sikeresen elküldve 868.1 MHz-en. A földi állomás fogadókész.*

---

### ➡️ Következő fejezet

A 8. fejezetben a CanSat **WiFi hálózaton is közvetít**: HTTP GET kéréssel valós időben láthatod az adatokat egy helyi szerveren. Megismered a `WiFiModule` API-ját és az offline fallback mintázatot.
