# 🛰️ Küldetési Napló — 7. fejezet: Rádiókapcsolat
## CSÜA Küldetésközpont · Földi Állomás Link

A kommunikációs osztály egy sürgős üzenetet küld: *"Az SD kártya jó biztonsági megoldás, de repülés közben valós idejű adatokat akarunk! Ha a CanSat fent van, tudni akarjuk, mi történik — azonnal, nem leszállás után."*

A megoldás: a **LoRa rádió**. Az RN2483 chip 868 MHz-en, több kilométeres hatótávolsággal sugároz. A jel átmegy épületeken, erdőkön, és a CanSat akár 40 km magasságból is el tudja érni a földi antennát.

Fizikailag a panelen egy apró kapcsoló van: **USB** vagy **Radio** állás. Fejlesztéskor (monitor, feltöltés) USB állásban kell lennie. Repülés előtt át kell kapcsolni **Radio** állásba — ettől kezdve a Serial port már nem a laptophoz, hanem az RN2483 chiphez csatlakozik.

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan küldd el az adatokat LoRa rádión (`radio.send`)
- Mi az a **LoRa** és miért alkalmas nagy távolságú kommunikációra
- Mit jelent a **1%-os duty cycle** (és miért kell a 2 másodperces várakozás)

---

### 💻 A kódod magyarázata

```cpp
void mission_loop() {
    float homerseklet  = sensors.temperature();
    float nyomas       = sensors.pressure();
    float paratartalom = sensors.humidity();

    radio.send(homerseklet, nyomas, paratartalom);
    ...
}
```

`radio.send(a, b, c)` — három `float` értéket küld el egyetlen LoRa csomagban, vesszővel elválasztva. Pl.:
```
23.40,1013.20,48.70
```
Ez a csomag jelenik meg a földi állomás vevőjén.

```cpp
    radio.send("Szöveg");
```
Ha szöveget akarsz küldeni (pl. esemény jelzéséhez), használd ezt a változatot.

**Miért kell a 2 másodperces várakozás?**  
A 868 MHz-es európai rádiósáv szabályzata szerint egy adó az idő legfeljebb **1%-ában** sugározhat (duty cycle). Egy LoRa csomag kb. 50 ms alatt megy át — tehát legalább 5 másodpercet kellene várni két csomag között. A framework 2 másodpercenként küld, ami pont belefér a korlátba.

**A kapcsoló teszt** (ha van hardware):
1. Fordítsd **Radio** állásba a kapcsolót
2. Futtasd `./cansat run`-t — a soros monitor nem fog semmit mutatni (mert a Serial a rádióhoz megy)
3. A földi vevőn megjelennek az adatok

---

### 🚀 Kihívás

**1. szintű kihívás:** Adj hozzá egy eseményjelet is! Ha a hőmérséklet 30°C fölé megy, küldd el szövegesen is:
```cpp
if (homerseklet > 30.0) {
    radio.send("MAGAS HOMERSEKLET");
}
```

**2. szintű kihívás:** Egyelőre 3 értéket küldünk. Mi lenne, ha a Z-gyorsulást is el akarod küldeni? A `radio.send` csak 3 paramétert vesz fel — hogyan oldanád meg? *(Tipp: `radio.send(String)` tetszőleges szöveget küld — pl. saját magad formázod a stringet.)*

**3. szintű kihívás — gondolkodós:** Ha a LoRa csomag vesszővel elválasztott adatokat küld, és a vevőn megkapjuk a `"23.40,1013.20,48.70"` szöveget — hogyan kellene ezt feldolgozni, hogy külön kapjuk meg a hőmérsékletet, légnyomást, páratartalmat?

---

### 🏆 Megszerzett jelvény

**📡 Kommunikációs Tiszt Jelvény**
*Az RN2483 LoRa rádió aktív és adatcsomagokat sugároz a földi állomás felé. A valós idejű telemetria elindult. A kommunikációs osztály megerősíti a kapcsolatot.*

---

### ➡️ Következő fejezet

A 8. fejezetben a CanSat csatlakozik a **WiFi hálózathoz** és adatokat küld egy HTTP szerverre — mint egy apró IoT eszköz! Ha az iskola hálózatán fut egy dashboard, valós időben látható lesz a CanSat telemetriája böngészőből.
