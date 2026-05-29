# 🛰️ Mérnöki Napló — 14. fejezet: Teljes Küldetés
## CSÜA Műszaki Divízió · Küldetésvezető Tanúsítvány ⭐

A Rendszertervező a csapat elé lép. *"Ez az utolsó feladat. A CanSat repülési szoftvere elvileg kész — de csak elvileg, amíg tesztelve nincs. A mai feladata: az összes fejezet anyagát integrálni egyetlen misszióba. Nincs részlegesen működő szoftver — minden alrendszernek működnie kell, és ha valamelyik hiányzik, arra fallback logikát kell írni."*

Ez a valódi mérnöki munka: nem egyes funkcionalitások tesztelése, hanem az integráció.

---

### 🎯 A teljes rendszer komponensei

| Alrendszer | Forrás | Státusz |
|---|---|---|
| BME280 légköri szenzor | 2. fejezet | ✓ |
| Barometrikus magasság | 3. fejezet | ✓ |
| LSM6DSOX IMU | 4. fejezet | ✓ |
| EMA szűrők (nyomás + az) | 11/13. fejezet | ✓ |
| Storage (SD CSV + events) | 6. fejezet | ✓ |
| Radio (LoRa 868 MHz) | 7. fejezet | ✓ |
| WiFi HTTP telemetria | 8. fejezet | ✓ |
| Repülési állapotgép | 9. fejezet | ✓ |
| Többsebességű ütemező | 12. fejezet | ✓ |

---

### 💻 Architektúra áttekintés

```
loop()  [fut ~10 000×/mp]
  │
  ├─ [100 ms] szenzor olvasás + EMA frissítés + updatePhase()
  ├─ [500 ms] Storage::log(rec)
  ├─ [2000 ms] radio.send() + wifi.send()
  └─ [1000 ms] Serial státusz

updatePhase()
  ├─ READY    → ASCENT   : ema_az > 2.0g
  ├─ ASCENT   → DESCENT  : ema_p > minPressure + 3 hPa  [APOGEE esemény]
  └─ DESCENT  → LANDED   : ema_p ≈ talaj + stabil az
```

**EMA szűrők:** A szenzor nyers adata csak az EMA frissítésére kerül — az állapotgép és a payload mindig a szűrt értéket kapja.

**Payload formátum (5 byte):**
```
[T_MSB][T_LSB][P_MSB][P_LSB][Phase]
```
A `phase` byte értéke: 0=READY, 1=ASCENT, 2=DESCENT, 3=LANDED.

**Fallback stratégiák:**
- WiFi nem elérhető → LoRa + SD mód
- SD kártya hiányzik → figyelmeztetés, de repülés folytatódik
- BME280 init fail → halt (nincs értelmű mérés nélkül repülni)
- Radio init fail → halt (nem teljesíti a küldetési követelményt)

---

### 🚀 Kihívás

**1. szint:** Teszteld az összes fallback ágat: húzd ki az SD kártyát, majd az I2C kábelt — mit ír ki a program? Mindig indokolt-e a `while(1)` leállítás, vagy lehetne folytatni?

**2. szint:** Bővítsd a `updatePhase()`-t egy **EMERGENCY** fázissal: ha az IMU 5 egymást követő mintában > 3.0g-t jelez DESCENT fázisban (ejtőernyő nem nyílt ki), lépj EMERGENCY-be és küldj 1 mp-enként `{'E','M','E','R','G'}` rádiócsomagot.

**3. szint — tervezési feladat:** Ez a szoftver jelenleg ~200 sor. Hogyan szerveznéd át modulokba, ha a projekt 10× nagyobb lenne? Tervezd meg a fájlstruktúrát: `mission/launch_detector.hpp`, `mission/apogee_detector.hpp`, `mission/telemetry.hpp` stb. Mit tartalmaz mindegyik, és hogyan kommunikálnak egymással?

> **A teljes misszió tesztelési ellenőrző lista:**
> - [ ] BME280 inicializálva, adatot ad
> - [ ] LSM6DSOX inicializálva, 6 tengely olvas
> - [ ] Rádió konfigurálva, TX látható a Serialon
> - [ ] SD munkamenet létrehozva (S00X mappa)
> - [ ] WiFi csatlakozik (vagy graceful fallback)
> - [ ] READY fázisban indul
> - [ ] Az asztal megütésekor NEM lép ASCENT-be
> - [ ] Fokozatos emelés szimulációnál ASCENT-be lép
> - [ ] EMA szűrő értékek szűk sávban vannak nyugalomban

---

### 🏆 Megszerzett jelvény

**🌟 Küldetésvezető Tanúsítvány**
*Az összes alrendszer integrálva, tesztelve és repülésre alkalmas állapotban. Teljes autonóm repülési szoftver: felszállásdetektálás, tetőpontmérés, multi-rate telemetria, zajszűrés — egy koherens rendszerben. A CSÜA Műszaki Divízió minősített küldetésvezetőjévé váltál.*

---

### ✅ A program elvégzéséről

Gratulálunk! A CSÜA Intermediate Szoftvermérnök Kurzus 14 fejezetét sikeresen teljesítetted. Megszerzett kompetenciák:

- Beágyazott C++ a valóságban (Arduino keretrendszer, PlatformIO)
- I2C perifériák kezelése drivereken keresztül
- Állapotgép tervezés és implementáció
- Valósidejű szignálfeldolgozás (EMA, SMA, gyűrűpuffer)
- Többfeladatos ütemezés `millis()`-alapon
- LoRa bináris protokoll és WiFi HTTP telemetria
- Hibatűrő szoftver tervezés fallback stratégiákkal

**Következő szint:** CSÜA Advanced — RTOS, GPS integráció, alapvető navigáció és saját protokoll tervezés.
