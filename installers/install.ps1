# Installs the mecseksat CLI. Public, no key needed — a school key is only
# needed later, for `mecseksat login`.
#
# Usage:
#   irm https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.ps1 | iex

$ErrorActionPreference = "Stop"

$RepoRaw = "https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main"
$InstallDir = if ($env:MECSEKSAT_INSTALL_DIR) { $env:MECSEKSAT_INSTALL_DIR } else { "$env:LOCALAPPDATA\mecseksat" }
$ScriptPath = Join-Path $InstallDir "mecseksat.py"
$ShimPath = Join-Path $InstallDir "mecseksat.cmd"

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Host "Python not found. mecseksat needs it (PlatformIO needs it too)."
    Write-Host "Install Python 3, then re-run this installer."
    exit 1
}

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

Write-Host "==> Downloading mecseksat..."
Invoke-WebRequest -Uri "$RepoRaw/cli/mecseksat.py" -OutFile $ScriptPath

# Windows has no shebang support for running .py files as a bare command, so
# drop a tiny .cmd shim on PATH that forwards to `python`.
@"
@echo off
python "%~dp0mecseksat.py" %*
"@ | Set-Content -Path $ShimPath -Encoding ASCII

Write-Host "==> Installed to $InstallDir"

$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($userPath -notlike "*$InstallDir*") {
    [Environment]::SetEnvironmentVariable("Path", "$userPath;$InstallDir", "User")
    Write-Host "==> Added $InstallDir to your user PATH. Restart your terminal for it to take effect."
} else {
    Write-Host "==> $InstallDir is already on your PATH."
}

Write-Host "==> Run 'mecseksat help' to get started (restart your terminal first if PATH just changed)."
