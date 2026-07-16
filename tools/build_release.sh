#!/usr/bin/env bash
# Packages the open-source framework for a GitHub Release. The mecseksat CLI
# and installers are fetched straight from the main branch by
# installers/install.sh|ps1 — no release process needed for those.
# cansat-edu-lib (Arduino) and cansat-edu-lib-idf (ESP-IDF) each need a
# versioned, releases/latest/download-able zip, since `mecseksat get` fetches
# them openly (no key) alongside gated begginer/intermediate/advanced lessons.
#
# Usage: ./tools/build_release.sh [outdir]
set -euo pipefail
cd "$(dirname "$0")/.."

OUT="${1:-dist}"
mkdir -p "$OUT"
rm -f "$OUT/cansat-edu-lib.zip" "$OUT/cansat-edu-lib-idf.zip"

zip -r -X "$OUT/cansat-edu-lib.zip" cansat-edu-lib -x '*.pyc' -x '*/__pycache__/*' >/dev/null
zip -r -X "$OUT/cansat-edu-lib-idf.zip" cansat-edu-lib-idf -x '*.pyc' -x '*/__pycache__/*' >/dev/null

echo "Built $OUT/cansat-edu-lib.zip and $OUT/cansat-edu-lib-idf.zip"
echo ""
echo "Upload them to a GitHub Release (tag the repo first: git tag vYYYY.MM.DD):"
echo "  gh release create vYYYY.MM.DD $OUT/cansat-edu-lib.zip $OUT/cansat-edu-lib-idf.zip --title vYYYY.MM.DD --notes '...'"
echo "  # or, to add to an existing release:"
echo "  gh release upload <tag> $OUT/cansat-edu-lib.zip $OUT/cansat-edu-lib-idf.zip"
