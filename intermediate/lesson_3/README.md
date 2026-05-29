# 🛰️ Mérnöki Napló — 3. fejezet: Magasságszámítás
## CSÜA Műszaki Divízió · Barometrikus Navigáció

A légköri osztályról friss számítás érkezik.

*"A CanSat maximális magassága 1000 méter lesz. A GPS nehéz és drága — a barometrikus módszer elegendő. Az ISA (International Standard Atmosphere) barometrikus formuláját kell implementálnod, és talaj-kalibrációval kompenzálni a helyszíni légnyomásváltozást. Az eredmény ±5 méteres pontosságú kell legyen stabil időjárásban."*

Előveszed a fizika-tankönyvet. Az egyenlet egyszerű — de a kalibrációs lépés sokat számít.

---

### 🎯 Tanulási célok

- A barometrikus magasságformula és fizikai háttere
- `powf()` és `<math.h>` használata
- Talaj-referencia kalibrálás `setup()`-ban
- Statikus (`static`) globális változók szerepe
- A mérési pontosság korlátai — mikor nem megbízható a baromérter?

---

### 💻 Kódmagyarázat

```cpp
float pressureToAltitude(float pressure_hpa) {
    return 44330.0f * (1.0f - powf(pressure_hpa / groundPressure, 0.1903f));
}
```

Ez az **ISA barometrikus formula**:

```
h = 44330 · (1 − (P / P₀)^0.1903)
```

- `P` — a mért légnyomás [hPa]
- `P₀` — a talaj-referencia légnyomás [hPa]
- `0.1903` = `R·T / (M·g)` — a légköri skálakonstans (szabad légkörben közelítő érték)
- `44330 m` — az elméleti légköri skálamagasság

```cpp
groundPressure = bme.readPressure() / 100.0F;
```

A `setup()`-ban egyszer leolvassuk az aktuális légnyomást és **talaj-referenciaként** eltároljuk. Így a formula mindig a felszállási ponthoz képesti relatív magasságot adja — nem a tengerszinthez képestit.

**Miért kell a `static` kulcsszó?**
```cpp
static float groundPressure = 1013.25f;
```
A `static` itt azt jelenti, hogy ez a változó a **teljes program életciklusa alatt** megmarad (nem semmisül meg a függvényhívás végén). Globális szinten ez a szokásos módja a modul-szintű állapottárolásnak.

---

### 🚀 Kihívás

**1. szint:** Írd ki a magasságot egész méterben (`(int)altitude`) és tizedessel is — hasonlítsd össze az értékeket. Melyik pontosabb a CanSat céljaira?

**2. szint:** Számítsd ki és írd ki a nyomás deriváltját: az előző méréshez képest mennyit változott a nyomás? Pozitív → emelkedés, negatív → süllyedés. Ehhez kell egy `prevPressure` változó.

**3. szint — gondolkodós:** A formula feltételezi a konstans hőmérsékletet. A valóságban a hőmérséklet a magassággal változik (standard légköri gradiens: −6.5°C/km). Keress rá a hőmérsékletkompenzált barometrikus formulára — miben különbözik ettől? Mikor kellene használni?

---

### 🏆 Megszerzett jelvény

**📐 Magasságmérő Mérnöki Tanúsítvány**
*A barometrikus navigációs alrendszer kalibrálva és tesztelve. Relatív magasság számítható ±5 m pontossággal. A repülési szimuláció kész.*

---

### ➡️ Következő fejezet

A 4. fejezetben a CanSat „érezni" fog: az **LSM6DSOX** chip gyorsulást és szögsebességet mér. Megismered az Adafruit LSM6DS library API-ját és a `sensors_event_t` struktúrát, és megtanulod, mit jelent a „g egység" a valóságban.
