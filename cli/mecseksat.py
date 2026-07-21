#!/usr/bin/env python3
"""mecseksat — global CLI for the MecsekSat CanSat Edu curriculum.

Stdlib-only by design: PlatformIO already requires Python on every student
machine, so this adds zero new runtime dependencies. Works on
Windows/macOS/Linux without modification.

Install:
  curl -sSL https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.sh | bash
  (Windows: irm https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.ps1 | iex)

Source: https://github.com/CosMIKLab/cansat-edu (this file, cansat-edu-lib, and the
installers are open source — lesson content is separate and requires a school key,
see 'mecseksat login').
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
import urllib.error
import urllib.request
import zipfile
from pathlib import Path

LESSON_SERVER_BASE = os.environ.get("MECSEKSAT_SERVER", "https://mecseksat.mik.pte.hu").rstrip("/")
FRAMEWORK_BASE = os.environ.get(
    "MECSEKSAT_FRAMEWORK_BASE",
    "https://github.com/CosMIKLab/cansat-edu/releases/latest/download",
).rstrip("/")

CRED_PATH = Path.home() / ".mecseksat" / "credentials"

PROTECTED_FILE = {
    "begginer": "src/mission.cpp",
    "intermediate": "src/mission.cpp",
    "advanced": "src/main.cpp",
}


def load_key():
    if not CRED_PATH.exists():
        return None
    return CRED_PATH.read_text().strip() or None


def save_key(key):
    CRED_PATH.parent.mkdir(parents=True, exist_ok=True)
    CRED_PATH.write_text(key.strip() + "\n")
    CRED_PATH.chmod(0o600)


def api_get(path, auth=False, timeout=15):
    url = f"{LESSON_SERVER_BASE}{path}"
    req = urllib.request.Request(url)
    if auth:
        key = load_key()
        if not key:
            print("Not logged in. Run: mecseksat login <key>")
            sys.exit(1)
        req.add_header("Authorization", f"Bearer {key}")
    try:
        return urllib.request.urlopen(req, timeout=timeout)
    except urllib.error.HTTPError as exc:
        if exc.code == 401:
            print("Invalid or missing key. Run: mecseksat login <key>")
        elif exc.code == 403:
            print("Your key has expired or isn't authorized for this lesson.")
        elif exc.code == 404:
            print("Not found.")
        else:
            print(f"Server error ({exc.code}).")
        sys.exit(1)
    except urllib.error.URLError as exc:
        print(f"Could not reach {LESSON_SERVER_BASE} ({exc.reason}).")
        sys.exit(1)


def fetch_manifest():
    with api_get("/lessons/v1") as resp:
        return json.load(resp)


def find_lesson(manifest, track, number):
    for entry in manifest["lessons"]:
        if entry["track"] == track and entry["number"] == number:
            return entry
    return None


def parse_spec(spec):
    parts = spec.split("/")
    if len(parts) != 2:
        return None
    track, number_str = parts
    try:
        return track, int(number_str)
    except ValueError:
        return None


def cmd_login(args):
    save_key(args.key)
    print("Saved. Verifying...")
    manifest = fetch_manifest()
    print(f"OK — {len(manifest['lessons'])} lesson(s) available on this account.")


def cmd_list(args):
    manifest = fetch_manifest()
    by_track = {}
    for entry in manifest["lessons"]:
        by_track.setdefault(entry["track"], []).append(entry)
    for track in ("begginer", "intermediate", "advanced"):
        lessons = by_track.get(track)
        if not lessons:
            continue
        print(f"\n{track}:")
        for e in sorted(lessons, key=lambda x: x["number"]):
            print(f"  {e['number']:>2}. {e['title']}")
    print(f"\nDownload one with: mecseksat get <track>/<number>  (e.g. mecseksat get begginer/1)")


def _safe_extractall(zf, dest):
    """extractall() with a zip-slip guard: refuses to extract if any member's
    resolved path would land outside dest (e.g. via '../' in the entry name)."""
    dest = Path(dest).resolve()
    for member in zf.infolist():
        target = (dest / member.filename).resolve()
        if target != dest and dest not in target.parents:
            raise ValueError(f"Unsafe path in zip, refusing to extract: {member.filename}")
    zf.extractall(dest)


def _extract_lesson_files(src, dest, skip):
    for root, _dirs, files in os.walk(src):
        for name in files:
            abs_src = Path(root) / name
            rel = abs_src.relative_to(src)
            rel_str = str(rel).replace(os.sep, "/")
            if rel_str in skip:
                continue
            target = dest / rel
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(abs_src, target)


def download_zip_bytes(track, number):
    with api_get(f"/lessons/v1/{track}/{number}", auth=True) as resp:
        return resp.read()


FRAMEWORK_LIBS = {
    "begginer": ["cansat-edu-lib"],
    "intermediate": ["cansat-edu-lib"],
    "advanced": ["cansat-edu-lib-idf"],
}


def ensure_shared_lib(cwd_root, lib_name):
    """Make sure <lib_name>/ exists next to the track dirs, fetched openly
    (no key) from the public framework releases."""
    lib_dir = cwd_root / lib_name
    if lib_dir.exists():
        return
    print(f"==> Fetching shared framework ({lib_name}, public, no key needed)...")
    url = f"{FRAMEWORK_BASE}/{lib_name}.zip"
    try:
        with urllib.request.urlopen(url, timeout=30) as resp:
            data = resp.read()
    except (urllib.error.HTTPError, urllib.error.URLError) as exc:
        print(f"Could not fetch {lib_name} ({exc}). You'll need it to build these lessons.")
        return
    with tempfile.TemporaryDirectory() as tmp:
        zip_path = Path(tmp) / "lib.zip"
        zip_path.write_bytes(data)
        with zipfile.ZipFile(zip_path) as zf:
            _safe_extractall(zf, cwd_root)


def ensure_shared_libs(track, cwd_root):
    for lib_name in FRAMEWORK_LIBS.get(track, []):
        ensure_shared_lib(cwd_root, lib_name)


def cmd_get(args):
    parsed = parse_spec(args.spec)
    if not parsed:
        print("Usage: mecseksat get <track>/<lesson-number>, e.g. mecseksat get begginer/1")
        sys.exit(1)
    track, number = parsed

    manifest = fetch_manifest()
    entry = find_lesson(manifest, track, number)
    if not entry:
        print(f"No such lesson: {track}/{number}")
        sys.exit(1)

    data = download_zip_bytes(track, number)
    cwd = Path.cwd()
    with tempfile.TemporaryDirectory() as tmp:
        zip_path = Path(tmp) / "lesson.zip"
        zip_path.write_bytes(data)
        extract_dir = Path(tmp) / "extracted"
        with zipfile.ZipFile(zip_path) as zf:
            _safe_extractall(zf, extract_dir)
        src = extract_dir / track / entry["dir_name"]
        if not src.exists():
            print(f"Downloaded archive did not contain {track}/{entry['dir_name']}; aborting.")
            sys.exit(1)
        dest = cwd / track / entry["dir_name"]
        dest.mkdir(parents=True, exist_ok=True)
        _extract_lesson_files(src, dest, skip=set())

    ensure_shared_libs(track, cwd)

    rel_dest = dest.relative_to(cwd)
    print(f"==> Downloaded to {rel_dest}")
    print(f"    cd {rel_dest} && mecseksat run")


def require_lesson_meta():
    meta_path = Path(".cansat-lesson.json")
    if not meta_path.exists():
        print("Not inside a downloaded lesson directory (no .cansat-lesson.json found).")
        sys.exit(1)
    return json.loads(meta_path.read_text())


def cmd_update(args):
    meta = require_lesson_meta()
    manifest = fetch_manifest()
    entry = find_lesson(manifest, meta["track"], meta["lesson"])
    if not entry:
        print("This lesson is no longer listed on the server.")
        sys.exit(1)
    if entry["version"] == meta["version"]:
        print(f"==> Already up to date ({meta['version']}).")
        return

    protected = PROTECTED_FILE[meta["track"]]
    print(f"==> Updating ({meta['version']} -> {entry['version']}), keeping your {protected}...")

    data = download_zip_bytes(meta["track"], meta["lesson"])
    with tempfile.TemporaryDirectory() as tmp:
        zip_path = Path(tmp) / "lesson.zip"
        zip_path.write_bytes(data)
        extract_dir = Path(tmp) / "extracted"
        with zipfile.ZipFile(zip_path) as zf:
            _safe_extractall(zf, extract_dir)
        src = extract_dir / meta["track"] / meta["dir_name"]
        if not src.exists():
            print(f"Downloaded archive did not contain {meta['track']}/{meta['dir_name']}; aborting.")
            sys.exit(1)
        _extract_lesson_files(src, Path("."), skip={protected, ".cansat-lesson.json"})

    ensure_shared_libs(meta["track"], Path("..") / "..")  # best-effort, only fetches if missing

    meta["version"] = entry["version"]
    Path(".cansat-lesson.json").write_text(json.dumps(meta, indent=2))
    print(f"==> Updated to {entry['version']}.")


def require_pio():
    if shutil.which("pio") is None:
        print("PlatformIO not found. Install it at: https://platformio.org/install/cli")
        sys.exit(1)


def cmd_run(args):
    require_pio()
    print("==> Building and flashing your mission...")
    if subprocess.run(["pio", "run", "--target", "upload"]).returncode == 0:
        print("==> Opening serial monitor (Ctrl+C to quit)...")
        subprocess.run(["pio", "device", "monitor"])


def cmd_build(args):
    require_pio()
    print("==> Building (checking for errors)...")
    subprocess.run(["pio", "run"])


def cmd_monitor(args):
    require_pio()
    print("==> Opening serial monitor (Ctrl+C to quit)...")
    subprocess.run(["pio", "device", "monitor"])


def cmd_check(args):
    meta = require_lesson_meta()
    if meta["track"] != "begginer":
        print("mecseksat check is only available on the begginer track.")
        sys.exit(1)
    require_pio()
    mission = Path("src/mission.cpp")
    backup = Path("src/mission.cpp.bak")
    check_src = Path("../../cansat-edu-lib/check/check_hardware.cpp")
    if not check_src.exists():
        print("cansat-edu-lib not found next to this lesson — run 'mecseksat get' again.")
        sys.exit(1)
    shutil.copy2(mission, backup)
    shutil.copy2(check_src, mission)
    try:
        print("==> Hardver ellenorzes indul...")
        if subprocess.run(["pio", "run", "--target", "upload"]).returncode == 0:
            print("==> Figyelj a kimenetre, majd nyomj Ctrl+C-t.")
            subprocess.run(["pio", "device", "monitor"])
    finally:
        shutil.copy2(backup, mission)
        backup.unlink()
        print("==> Sajat kodod visszaallitva.")


def cmd_new(args):
    meta = require_lesson_meta()
    if meta["track"] == "advanced":
        print("The advanced track has no starter template — edit src/main.cpp directly.")
        sys.exit(1)
    template = Path("mission_template.cpp")
    if not template.exists():
        print("mission_template.cpp not found in this lesson.")
        sys.exit(1)
    shutil.copy2(template, "src/mission.cpp")
    print("==> mission.cpp has been reset to the starter template.")


def cmd_help(args):
    print(
        """
  mecseksat — MecsekSat CanSat Edu CLI

  Account:
    mecseksat login <key>     Save your school's access key
    mecseksat list            Show available lessons (no login needed)
    mecseksat get <t>/<n>     Download a lesson, e.g. mecseksat get begginer/1

  Inside a downloaded lesson directory:
    mecseksat run             Build, flash, then open serial monitor
    mecseksat build           Build only (check for compile errors)
    mecseksat monitor         Open serial monitor
    mecseksat check           Hardware diagnostic (begginer only)
    mecseksat new             Reset to the starter template (begginer/intermediate)
    mecseksat update          Download the latest version (keeps your code)
    mecseksat help            Show this message
"""
    )


def build_parser():
    parser = argparse.ArgumentParser(prog="mecseksat", add_help=False)
    sub = parser.add_subparsers(dest="command")

    p_login = sub.add_parser("login")
    p_login.add_argument("key")
    p_login.set_defaults(func=cmd_login)

    sub.add_parser("list").set_defaults(func=cmd_list)

    p_get = sub.add_parser("get")
    p_get.add_argument("spec")
    p_get.set_defaults(func=cmd_get)

    sub.add_parser("update").set_defaults(func=cmd_update)
    sub.add_parser("run").set_defaults(func=cmd_run)
    sub.add_parser("build").set_defaults(func=cmd_build)
    sub.add_parser("monitor").set_defaults(func=cmd_monitor)
    sub.add_parser("check").set_defaults(func=cmd_check)
    sub.add_parser("new").set_defaults(func=cmd_new)
    sub.add_parser("help").set_defaults(func=cmd_help)
    return parser


def main():
    args = build_parser().parse_args()
    func = getattr(args, "func", None)
    if not func:
        cmd_help(args)
        return
    func(args)


if __name__ == "__main__":
    main()
