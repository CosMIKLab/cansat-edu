# 🛰️ Küldetési Napló — 8. fejezet: WiFi kapcsolat
## CSÜA Küldetésközpont · Telemetriai Panel

A technológiai osztályról egy üzenet érkezik: *"A rádió kiváló repüléshez, de a tesztek során — amikor a CanSat még az asztalon van — WiFi-n is el tudja küldeni az adatokat egy dashboardra. Így az egész iskola láthatja valós időben a méréseket a böngészőjéből!"*

Az ESP8266 mikrovezérlőnek beépített WiFi modulja van — nem kell hozzá semmilyen extra hardver. A `wifi.connect()` hívásra csatlakozik a hálózathoz, a `wifi.send()` hívásra pedig elküld egy HTTP GET kérést — pontosan úgy, mint amikor a böngésző megnyit egy weboldalt.

A feladatod: csatlakoztatni a CanSat-ot a WiFi hálózathoz, és elküldeni az adatokat egy szervercímre.

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan csatlakozzon a CanSat WiFi hálózathoz (`wifi.connect`)
- Hogyan küldj adatokat HTTP GET kéréssel (`wifi.send`)
- Hogyan ellenőrizd, hogy van-e aktív kapcsolat (`wifi.connected`)
- Mi az a **HTTP GET** kérés és URL paraméter

---

### 💻 A kódod magyarázata

```cpp
void mission_setup() {
    Serial.println("WiFi kapcsolat teszt...");
    wifi.connect("IskolaHalozat", "jelszo123");
}
```

`wifi.connect(hálózat_neve, jelszó)` — csatlakozik a WiFi hálózathoz. Ez **legfeljebb 10 másodpercig blokkolhat** (vár a kapcsolódásra). Siker esetén kiírja az IP-címet a Serial monitorra. Ezért a `mission_setup`-ban kell meghívni — egyszer, induláskor.

Cseréld ki `"IskolaHalozat"` és `"jelszo123"` értékeket a valódi hálózat adataira!

```cpp
void mission_loop() {
    ...
    if (wifi.connected()) {
        wifi.send("http://192.168.1.5/adatok", homerseklet, nyomas, paratartalom);
        Serial.println("WiFi adatok elküldve a dashboardra!");
    } else {
        Serial.println("Nincs WiFi kapcsolat...");
    }
    ...
}
```

`wifi.connected()` — `true` (igaz), ha van aktív WiFi kapcsolat.

`wifi.send(url, a, b, c)` — HTTP GET kérést küld az `url` címre, ehhez hozzáfűzve a három paramétert:
```
GET http://192.168.1.5/adatok?a=23.40&b=1013.20&c=48.70
```
A szerver fogadja ezt a kérést, és az `a`, `b`, `c` értékekből megjeleníti a mérési adatokat.

Az `if (wifi.connected())` ellenőrzés azért fontos, mert ha nincs WiFi, a `wifi.send` egyszerűen nem csinál semmit — de jobb, ha tudjuk, hogy miért nem jönnek az adatok.

---

### 🚀 Kihívás

**1. szintű kihívás:** Írd be a saját iskolai WiFi hálózatod adatait (kérd meg a tanárod), majd futtasd. A Serial monitoron meg kell jelennie az IP-címnek, ha sikerült a kapcsolódás.

**2. szintű kihívás:** Módosítsd a kódot: csak akkor küldj WiFi-n, ha a hőmérséklet 25°C fölé megy. Hidegebb esetén ne küldjön semmit. *(Kombináld az `if` feltételt a `wifi.connected()`-del: beágyazott if-ek vagy `&&` operátor.)*

**3. szintű kihívás — gondolkodós:** A WiFi-en küldött URL-ben az adatok `a`, `b`, `c` névvel szerepelnek. Ha egy valódi dashboardot programoznál, miért lenne jobb, ha `temp`, `pressure`, `humidity` lenne a nevük? *(Erre a `radio.send(String)` mintáját követve lehetne egyedi URL-t összerakni.)*

---

### 🏆 Megszerzett jelvény

**📶 Telemetriai Rendszermérnök Jelvény**
*A WiFi modul aktív, az adatok valós időben érik el a dashboard szervert. Az iskola hálózatán mostantól böngészőből is követhető a CanSat telemetriája.*

---

### ➡️ Következő fejezet

A 9. fejezetben a CanSat igazi intelligenciát kap: automatikusan **felismeri a repülési fázisokat** — felszállást, tetőpontot. Megtanulod, hogyan tárolj állapotot a program futása közben (`bool` változók), és hogyan épül fel egy egyszerű állapotgép.
