# Letöltési útmutató — mecseksat CLI

Ez az útmutató megmutatja, hogyan telepítsd a `mecseksat` parancssori eszközt, és
hogyan tölts le vele egy-egy leckét anélkül, hogy az egész repót klónoznod kellene.

A `mecseksat` eszköz maga (és a `cansat-edu-lib` keretrendszer) nyílt forráskódú.
A tananyag tartalma (leckeleírások, feladatok) egy iskolai kulccsal érhető el —
ezt az iskolád tanárától vagy a MecsekSat csapattól kapod.

---

## 1. Telepítés

### Mac / Linux

```bash
curl -sSL https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.sh | bash
```

### Windows (PowerShell)

```powershell
irm https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.ps1 | iex
```

Mindkét telepítő a `python3`/`python` parancsot használja — mivel a PlatformIO-hoz
amúgy is szükséged lesz Python 3-ra, ez nem jelent új függőséget. Ha a telepítő
Python hiányra panaszkodik, telepítsd a [Python 3](https://www.python.org/downloads/)-at,
majd futtasd újra a fenti parancsot.

Ellenőrizd, hogy sikerült-e:

```bash
mecseksat help
```

Ha a `mecseksat` parancs nem található, a telepítő kiírja, melyik mappát kell
hozzáadnod a `PATH`-hoz — kövesd az utasítást, majd nyiss új terminált.

---

## 2. Kulcs beszerzése és bejelentkezés

Az iskolád kap egy egyedi kulcsot (formátuma: `MSK-XXXXXXXXXX`), amely egy tanévre
érvényes, és minden leckéhez hozzáférést ad.

```bash
mecseksat login MSK-XXXXXXXXXX
```

A kulcs a gépeden `~/.mecseksat/credentials` fájlban tárolódik — nem kell újra
megadnod, amíg nem törlöd vagy nem cserélsz kulcsot.

---

## 3. Elérhető leckék listázása

Ez bejelentkezés nélkül is működik, hogy megnézhesd, mi érhető el:

```bash
mecseksat list
```

Kimenet pályánként csoportosítva, pl.:

```
begginer:
   1. Az első kapcsolat
   2. Hőmérséklet mérés
   ...

intermediate:
   1. Rendszerindítás
   ...
```

---

## 4. Egy lecke letöltése

```bash
mecseksat get begginer/1
```

Ez létrehozza a `begginer/lesson_1/` mappát a jelenlegi könyvtáradban a lecke
tartalmával (README, feladat, kiinduló kód). Kezdő pálya esetén automatikusan
letölti a `cansat-edu-lib` keretrendszert is (nyíltan, kulcs nélkül) egy szintbeli
`cansat-edu-lib/` mappába, mert a fordításhoz szükség van rá.

```bash
cd begginer/lesson_1
mecseksat run
```

Egész pálya letöltéséhez ismételd meg a leckeszámmal (`mecseksat get begginer/2`,
`mecseksat get begginer/3`, …) — nincs szükség az egész repó klónozására.

---

## 5. Parancsok egy letöltött lecke mappájában

```bash
mecseksat run       # Fordítás → feltöltés → serial monitor megnyitása
mecseksat build     # Csak fordítás (hibák ellenőrzése)
mecseksat monitor   # Serial monitor megnyitása (már feltöltött kódhoz)
mecseksat check     # Hardver-ellenőrzés (csak kezdő pálya)
mecseksat new       # Kód visszaállítása az eredeti sablonra (kezdő/haladó Arduino)
mecseksat update    # A lecke legújabb verziójának letöltése — a saját kódod megmarad
mecseksat help      # Parancsok listája
```

> **Fizikai kapcsoló:** Flashelés és monitor előtt legyen **USB** állásban. Repülés
> előtt állítsd át **Radio** állásba!

---

## 6. Frissítés

Ha egy lecke tartalma frissül (pl. hibajavítás a leírásban), a saját munkád
elvesztése nélkül tudod lehúzni az új verziót:

```bash
cd begginer/lesson_1
mecseksat update
```

Ez mindent frissít a leckében **kivéve** a saját kódodat (`src/mission.cpp`, illetve
az ESP-IDF pályán `src/main.cpp`) — azt sosem írja felül.

---

## 7. Gyakori hibák

**"Not logged in."** — Futtasd a `mecseksat login <kulcs>` parancsot.

**"Invalid or missing key."** — Ellenőrizd, hogy pontosan másoltad-e be a kulcsot
(nagybetű/kisbetű számít). Ha biztosan jó, kérdezd meg a tanárodat, hogy érvényes-e
még az iskola előfizetése.

**"Not inside a downloaded lesson directory."** — A `run`/`build`/`monitor`/`check`/
`new`/`update` parancsokat egy `mecseksat get`-tel letöltött lecke mappájából kell
futtatni (ott, ahol a `.cansat-lesson.json` fájl van).

**PlatformIO not found** — Telepítsd innen: <https://platformio.org/install/cli>

---

## Lásd még

| Fájl | Tartalom |
|------|----------|
| [begginer/overview.md](begginer/overview.md) | Kezdő pálya keretrendszer felépítése |
| [begginer/getting-started.md](begginer/getting-started.md) | Kezdő pálya első futtatás, CLI referencia |
| [begginer/api.md](begginer/api.md) | Kezdő pálya teljes API referencia |
| [base/overview.md](base/overview.md) | A referencia firmware (`base/`) felépítése |

