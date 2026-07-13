# MecsekSat — CanSat Edu (framework)

Nyílt forráskódú keretrendszer a **MecsekSat CanSat Edu** platformhoz — az ESP8266-os
panel driverei, a diákoknak szánt `mecseksat` parancssori eszköz, és a referencia
firmware.

**A tananyag maga (leckeleírások, feladatok) külön, iskolai kulccsal védett tartalom —
ez a repó csak a keretrendszert tartalmazza.** Lásd [`docs/download-guide.md`](docs/download-guide.md)
a lecke-letöltés menetéhez.

---

## Gyors start (diákoknak)

```bash
# Telepítsd a mecseksat parancssori eszközt
curl -sSL https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.sh | bash
# Windows PowerShell: irm https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.ps1 | iex

mecseksat login MSK-XXXXXXXXXX     # az iskoládtól kapott kulcs
mecseksat list                     # elérhető leckék
mecseksat get begginer/1           # egy lecke letöltése
cd begginer/lesson_1 && mecseksat run
```

Részletes útmutató: [`docs/download-guide.md`](docs/download-guide.md)

---

## Mi van ebben a repóban?

| Mappa | Tartalom |
|-------|----------|
| [`cli/`](cli/) | A `mecseksat` parancssori eszköz forráskódja (Python, stdlib-only) |
| [`installers/`](installers/) | `install.sh` / `install.ps1` — a fenti telepítők forrása |
| [`cansat-edu-lib/`](cansat-edu-lib/) | Megosztott Arduino/PlatformIO driver könyvtár (kezdő pálya kerete) |
| [`base/`](base/) | Referencia firmware — nyers driverek, nincs tananyag |
| [`docs/`](docs/) | Nyílt dokumentáció (lásd lent) |
| [`tools/`](tools/) | Karbantartói eszközök (framework release csomagolás) |

A tényleges leckék (kezdő/haladó Arduino/ESP-IDF pályák) egy külön, privát repóban
élnek, és a `mecseksat` CLI-n keresztül, iskolai kulccsal érhetők el — lásd fent.

---

## Hardver

| Komponens | Leírás |
|-----------|--------|
| ESP8266 ESP-12E | Mikrovezérlő, 80 MHz, 4 MB flash, beépített WiFi |
| BME280 | Hőmérséklet / légnyomás / páratartalom (I2C 0x76) |
| LSM6DSOX | 6-tengelyes IMU: gyorsulásmérő + giroszkóp (I2C 0x6A) |
| RN2483 | LoRa rádió, 868.1 MHz, SF7, UART AT-parancsok |
| SD kártya | Adatnaplózás, FAT32 |

Részletes kapcsolási rajz: [`docs/base/hardware.md`](docs/base/hardware.md)

---

## Dokumentáció

- [`docs/download-guide.md`](docs/download-guide.md) — hogyan telepítsd a `mecseksat`-ot és tölts le egy leckét
- [`docs/base/`](docs/base/) — Nyers driver API (bme280, lsm6dsox, radio)
- [`docs/begginer/`](docs/begginer/) — Kezdő keretrendszer API (`cansat-edu-lib`) referencia

---

## Karbantartóknak

Framework-release csomagolás (`cansat-edu-lib.zip` a GitHub Releases-hez):

```bash
./tools/build_release.sh
```

Lásd [`docs/download-guide.md`](docs/download-guide.md) a diák oldali folyamathoz, és a
(privát) content repó `tools/README.md`-jét a lecke-tartalom csomagolásához.
