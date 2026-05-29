# 🛰️ Mérnöki Napló — 8. fejezet: WiFi Telemetria
## CSÜA Műszaki Divízió · Hálózati Kommunikáció

A tesztelési osztályról kérés érkezik: *"A beágyazott teszt alatt a CanSatot egy helyi hálózaton akarjuk monitorozni valós időben — dashboard-on. A LoRa rádió a tesztteremben nem megbízható (interferencia), de a WiFi igen. Implementáld a WiFi telemetriát úgy, hogy ha a hálózat nem elérhető, a kód gond nélkül offline módban fusson tovább."*

Ez az **offline-first** tervezési elv: a WiFi bónusz, nem alapfeltétel.

---

### 🎯 Tanulási célok

- `WiFiModule` API: `connect()`, `connected()`, `send()`
- HTTP GET paraméterek (`?a=&b=&c=`) és URL-felépítés
- Feltételes végrehajtás WiFi állapot alapján
- Offline fallback mintázat — a kód WiFi nélkül is fut
- Blokkoló vs. nem-blokkoló hálózati kapcsolat

---

### 💻 Kódmagyarázat

```cpp
wifi.connect("IskolaHalozat", "jelszo123");
```

A `connect()` **blokkoló**: legfeljebb 20 × 500ms = 10 másodpercig vár a kapcsolódásra, majd visszatér (sikerestől függetlenül). Ez a `setup()`-ban elfogadható, de a `loop()`-ban nem szabad hívni.

```cpp
if (wifi.connected()) {
    Serial.println("WiFi: csatlakozva — HTTP telemetria aktív");
} else {
    Serial.println("WiFi: nem elérhető — offline módban folytatás");
}
```

A `connected()` mindig ellenőrizhető — ha a kapcsolat közben megszakad, `false`-t ad vissza.

```cpp
if (wifi.connected()) {
    wifi.send("http://192.168.1.5/telemetria", rec.temp, rec.pressure, rec.humidity);
}
```

A `send()` belső implementációja:
```cpp
snprintf(full, 128, "%s?a=%.2f&b=%.2f&c=%.2f", url, a, b, c);
```
Az URL így épül fel: `http://192.168.1.5/telemetria?a=23.45&b=1013.25&c=65.00`

A szerver (pl. egy Python Flask app vagy Node.js) feldolgozza a `?a`, `?b`, `?c` query paramétereket. A `b` és `c` szabadon felhasználható bármilyen értékre.

**Offline fallback:** Mivel a `wifi.send()` előtt ellenőrzünk `wifi.connected()`-et, az egész hálózati rész kihagyható WiFi nélkül. A program hibátlanul fut tovább LoRa + SD módban.

**Fontos:** A `wifi.hpp` fejlécben a `WiFi` névütközés elkerülésére a `WiFiModule` osztálynevet használjuk — az ESP8266 SDK-ban már van egy `WiFi` globális objektum.

---

### 🚀 Kihívás

**1. szint:** Indíts el egy egyszerű Python HTTP szervert a számítógépeden:
```bash
python3 -m http.server 80
```
Majd módosítsd az URL-t `http://<PC_IP>/telemetria`-ra. Nézd meg a szerver logját — látod a GET kéréseket?

**2. szint:** Bővítsd a `send()` hívást az `az` értékkel. A jelenlegi API csak 3 paramétert vesz — hogyan oldanád meg ezt a korlátot? (Például: URL-t manuálisan formázz `snprintf`-fel, és `wifi.send()`-et ne használj, hanem a belsejébe nézz.)

**3. szint — gondolkodós:** A HTTP GET kérés plaintext és logolható. Milyen biztonsági problémák vannak ezzel? Mikor kellene HTTPS-t vagy legalább alap autentikációt alkalmazni egy valódi misszióban?

---

### 🏆 Megszerzett jelvény

**🌐 Hálózati Telemetria Jelvény**
*A WiFi HTTP kommunikációs alrendszer implementálva, offline fallbackkel biztosítva. Valós idejű dashboard-monitorozás lehetséges helyi hálózaton.*

---

### ➡️ Következő fejezet

A 9. fejezetben a CanSat „tudni fogja, hol tart a repülésben": **állapotgép** segítségével kezeli a READY → ASCENT → APOGEE → DESCENT → LANDED átmeneteket. Megismered az `enum class` típust és a `switch/case` szerkezetet.
