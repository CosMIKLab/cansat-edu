# 🛰️ Küldetési Napló — 2. fejezet: Hőmérséklet mérés
## CSÜA Küldetésközpont · Érzékelő Teszt I.

A főmérnök megint odajön az asztalodhoz, és elégedett mosollyal bólint: *"Rendben, az első kapcsolatfelvétel sikeres volt. Most komolyabb feladatot kapsz."*

A légköri osztályról érkező üzenet szerint a CanSat hamarosan a **sztratoszféra határára** fog repülni, ahol a hőmérséklet akár **–60 °C** is lehet. A fedélzeti elektronika is csak bizonyos hőmérsékleti tartományban működik megbízhatóan. Ezért az első igazi mérés: a hőmérséklet.

*"A BME280-as szenzor már be van kötve a panelre"*, magyarázza a mérnök. *"A te dolgod csak annyi, hogy lekérd az értéket, és megjelenítsd a monitoron. Egy sor kód az egész."*

Beülsz a munkaállomáshoz. Ideje megírni az első valódi mérést!

---

### 🎯 Ebben a leckében megtanulod:

- Mi az a `float` — hogyan tárolunk tizedes számokat a programban
- Hogyan olvasd le a hőmérsékletet (`sensors.temperature()`)
- Hogyan írj ki számot és szöveget egyszerre a monitorra

---

### 💻 A kódod magyarázata

```cpp
void mission_loop() {
    float homerseklet = sensors.temperature();
    ...
}
```

**`float`** — ez egy "lebegőpontos szám", azaz tizedes szám. A hőmérséklet pl. `23.45` — nem egész, ezért `float`-ot használunk.

**`homerseklet`** — ez a változó neve. Ide kerül az éppen mért érték. Bármilyen nevet adhatsz neki (szóköz nélkül, ékezet nélkül).

**`sensors.temperature()`** — ez olvassa le a BME280 szenzort és visszaadja a hőmérsékletet Celsius-fokban. A kapott értéket a `homerseklet` változóba tesszük.

```cpp
    Serial.print("Hőmérséklet: ");
    Serial.print(homerseklet);
    Serial.println(" °C");
```

- `Serial.print(...)` — kiírja, amit kap, de **nem vált sort** a végén
- `Serial.println(...)` — kiírja, és **sort vált** utána

Az első sor kiírja a feliratot (`Hőmérséklet: `), a második a számot, a harmadik az egységet és sort vált. Eredmény a monitoron:
```
Hőmérséklet: 23.45 °C
```

> **Fontos tudni:** A `sensors.temperature()` hívás egyszerre frissíti a légnyomás és páratartalom értékeket is (ezekről a 3. fejezetben tanulunk). Ezért mindig a hőmérsékletet érdemes elsőként lekérni.

---

### 🚀 Kihívás

**1. szintű kihívás:** Változtasd meg a változó nevét `homerseklet`-ről `temp`-re — minden helyen, ahol szerepel. Futtatás után ugyanúgy kell működnie.

**2. szintű kihívás:** Adj hozzá egy második `Serial.print` sort, ami kiírja, hogy meleg-e vagy hideg:
```
Hőmérséklet: 23.45 °C
(ez meleg!)
```
*(Ehhez még nem kell if — csak egy extra `Serial.println` sort adj hozzá.)*

**3. szintű kihívás — gondolkodós:** Mi lenne, ha `int homerseklet` helyett `float homerseklet`-et írnál? Próbáld ki! (`int` egész számot tárol — mi változik az eredményen?)

---

### 🏆 Megszerzett jelvény

**🌡️ Hőmérő Technikusi Jelvény**
*Sikeresen aktiváltad a BME280 hőmérséklet-szenzort és valós adatot olvastál le a CanSat fedélzetéről. A légköri osztály elégedett.*

---

### ➡️ Következő fejezet

A 3. fejezetben a BME280 összes értékét lekéred: hőmérséklet, **légnyomás** és **páratartalom** egyszerre. Megtanulod, hogyan kezeld egyszerre több változót, és hogyan formázd szépen az adatkiírást.
