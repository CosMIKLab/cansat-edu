# 🛰️ Mérnöki Napló — 5. fejezet: Vektoros fizika
## CSÜA Műszaki Divízió · Orientáció Analitika

A repülési szimulációban egy probléma merül fel: a CanSat elforoghat a tengelye körül repülés közben. Ezt nem az egyes tengelyértékekből, hanem a **teljes gyorsulásvektorból** kell meghatározni.

*"Szükségünk van egy dőlésszög-számítóra. Ha a CanSat 30 foknál többet dől el a függőlegestől, azt a telemetriai csomagban jelezni kell. Az eljárás: a gyorsulásvektor és a gravitációs irány (Z-tengely) szöge adja meg a dőlést."*

Előveszed a trigonometria könyvedet.

---

### 🎯 Tanulási célok

- Vektormagnitudó számítása (`sqrtf`, `<math.h>`)
- Dőlésszög kiszámítása `acosf` és vektordot-product alapján
- Saturation clamp: miért kell korlátok közé szorítani az `acosf` bemenetét
- Mikor „hazudik" a gyorsulásmérő — dinamikus vs. statikus gyorsulás
- Saját segédfüggvény írása C++-ban

---

### 💻 Kódmagyarázat

```cpp
float tiltAngle(float ax, float ay, float az) {
    float magnitude = sqrtf(ax*ax + ay*ay + az*az);
    if (magnitude < 0.01f) return 0.0f;
    float cos_z = az / magnitude;
    if (cos_z >  1.0f) cos_z =  1.0f;
    if (cos_z < -1.0f) cos_z = -1.0f;
    return acosf(cos_z) * 180.0f / M_PI;
}
```

**Lépésről lépésre:**

1. `sqrtf(ax²+ay²+az²)` — a vektor hossza (magnitudója). Álló CanSatnál ~1.0g.
2. `az / magnitude` — a Z-irányú egységvektor komponense. Ha a CanSat egyenesen áll, ez +1.0.
3. **Saturation clamp**: az `acosf` érvényes tartománya `[-1, +1]`. Lebegőpontos kerekítés miatt a vektor normalizálása néha 1.0001-t adhat — ez NaN (nem szám) hibát okozna. A clamp megvéd ettől.
4. `acosf(cos_z)` — az ívkoszinusz adja vissza a szöget **radiánban**. Szorzás `180/π`-vel adja a fokokat.

```cpp
sensors_event_t accel, gyro, temp_e;
imu.getEvent(&accel, &gyro, &temp_e);
float ax = accel.acceleration.x / 9.80665f;
float ay = accel.acceleration.y / 9.80665f;
float az = accel.acceleration.z / 9.80665f;
float mag = sqrtf(ax*ax + ay*ay + az*az);
```
Ha a CanSat **szabadesésben** van, a magnitudó ~0g közelit — mert a chip nem érez gravitációt (súlytalanság). Ez megkülönbözteti a valódi repülést az álló állapottól!

**A dőlésszög fizikai korlátja:** A gyorsulásmérő csak statikus esetben adja meg a pontos dőlésszöget. Gyorsulás közben (pl. rakétaindítás) a kapott szög hamis — mert a chip az összesített gyorsulást méri, nem csak a gravitációt.

---

### 🚀 Kihívás

**1. szint:** Adj hozzá egy `rollAngle()` függvényt, ami az X és Y tengely alapján kiszámítja a forgási szöget:
```cpp
float rollAngle = atan2f(ay, ax) * 180.0f / M_PI;
```
Mi a különbség az `atan2f` és az `acosf` megközelítés között?

**2. szint:** Szimulálj „instabilitás riasztást": ha a dőlés > 45 fok ÉS a magnitudó > 0.5g, küldd el az üzenetet Serialra: `"FIGYELEM: Instabil orientáció!"`. Ehhez kombinálj két feltételt `&&`-lal.

**3. szint — gondolkodós:** A gyorsulásmérő + barométer kombó vs. giroszkóp integráció: melyik ad jobb orientáció-becslést hosszú repülés alatt? Mik a korlátai a giroszkóp-integrációnak? (Kulcsszó: „gyroscopic drift")

---

### 🏆 Megszerzett jelvény

**📡 Vektorfizika Jelvény**
*A dőlésszög-analitika implementálva és tesztelve. A fedélzeti orientációs alrendszer képes detektálni az instabilis repülési helyzeteket.*

---

### ➡️ Következő fejezet

A 6. fejezetben az adatok nem tűnnek el a Serial monitoron — **SD kártyára mennek**. Megtanulod a `Storage` névtér API-ját, a CSV formátumot, és a `TelemetryRecord` struktúra összes mezőjét.
