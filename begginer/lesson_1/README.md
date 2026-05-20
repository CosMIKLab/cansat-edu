# 🛰️ Küldetési Napló — 1. fejezet: Az első kapcsolat
## CSÜA Küldetésközpont · Rendszer-ellenőrzés

Gratulálunk! Felvételt nyertél a **CSÜA Junior Küldetésirányítói Programjába** (CanSat Ügynökség). Ma a legelső napod a küldetésközpontban — és máris feladatot kapsz.

Az irányítóteremben a főmérnök fogad: *"Üdvözlöm, Junior Irányító! A CanSat műholdunk ma reggel érkezett meg a laborba. Mielőtt bármit csinálunk vele, el kell végezni az alapvető rendszer-ellenőrzést: be kell kapcsolni, és meg kell győződnünk róla, hogy a fedélzeti számítógép reagál. Ez az 1. lépés minden küldetés előtt."*

A kezedbe nyomnak egy USB kábelt és egy laptopot. A küldetés egyszerű: írd meg az első programot, töltsd fel a CanSat-ra, és nézd meg, hogy megjelenik-e a monitor képernyőjén az üzenet. Ha igen — a rendszer él!

---

### 🎯 Ebben a leckében megtanulod:

- Hogyan néz ki egy CanSat program (a két fő rész: `mission_setup` és `mission_loop`)
- Hogyan írj szöveget a monitorra (`Serial.println`)
- Hogyan töltsd fel és futtasd a kódot (`./cansat run`)

---

### 💻 A kódod magyarázata

```cpp
#include <cansat.h>
```
Ez az egyetlen sor, amit minden programod elejére be kell írni. Ez "betölti" az összes CanSat funkciót — szenzorok, rádió, SD kártya, minden.

```cpp
void mission_setup() {
    Serial.println("==============================");
    Serial.println("  CSÜA Küldetésirányítás     ");
    Serial.println("  Minden rendszer elindult!  ");
    Serial.println("==============================");
}
```
A `mission_setup()` **egyszer fut le**, közvetlenül bekapcsolás után. Ide kerülnek az egyszeri beállítások. Most csak üdvözlőszöveget írunk ki.

`Serial.println(...)` — szöveget ír a monitorra, majd sort vált. Amit a `"..."` idézőjelek közé írsz, az fog megjelenni.

```cpp
void mission_loop() {
    Serial.println("CanSat aktív — rendszerek OK");
}
```
A `mission_loop()` **minden 2 másodpercben lefut** újra és újra. Ez a CanSat "szíve" — ide kerül majd a mérés, az adatküldés, minden. Most csak egy állapotjelentést ír ki.

---

### 🚀 Kihívás

Módosítsd a `src/mission.cpp` fájlt, majd futtasd újra a `./cansat run` parancsot!

**1. szintű kihívás:** Írd át az üdvözlőüzenetet a saját nevedre, pl.:
```cpp
Serial.println("  Junior Irányító: Kovács Péter  ");
```

**2. szintű kihívás:** A `mission_loop`-ban adj hozzá egy másik sort, ami más üzenetet ír ki, pl.:
```cpp
Serial.println("Várjuk a szenzor adatokat...");
```

**3. szintű kihívás:** Mi történik, ha a `mission_setup`-ban és a `mission_loop`-ban is van `Serial.println`? Figyeld meg a monitort: melyik jelenik meg egyszer, és melyik ismétlődik!

> **Tipp:** Ha elrontottad a kódot és vissza szeretnél térni az eredeti állapotba, futtasd: `./cansat new`

---

### 🏆 Megszerzett jelvény

**⚡ Rendszer-Aktiváló Jelvény**
*Sikeresen elindítottad a CanSat fedélzeti számítógépét és megerősítötted, hogy a kommunikáció működik. A CSÜA minden Junior Irányítójának ez az első lépés.*

---

### ➡️ Következő fejezet

A 2. fejezetben az első igazi szenzort kapcsolod be: a **hőmérőt**. Megtanulod, hogyan kell egy számot eltárolni a programban, és hogyan olvasd le a valós hőmérsékletet a CanSat belsejéből.
