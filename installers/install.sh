#!/usr/bin/env bash
# Installs the mecseksat CLI. Public, no key needed — the CLI itself is open
# source; a school key is only needed later, for `mecseksat login`.
#
# Usage:
#   curl -sSL https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main/installers/install.sh | bash
set -euo pipefail

REPO_RAW="https://raw.githubusercontent.com/CosMIKLab/cansat-edu/main"
INSTALL_DIR="${MECSEKSAT_INSTALL_DIR:-$HOME/.local/bin}"
TARGET="$INSTALL_DIR/mecseksat"

if ! command -v python3 &>/dev/null; then
    echo "Python 3 not found. mecseksat needs it (PlatformIO needs it too)."
    echo "Install Python 3, then re-run this installer."
    exit 1
fi

mkdir -p "$INSTALL_DIR"
echo "==> Downloading mecseksat..."
curl -fsSL "$REPO_RAW/cli/mecseksat.py" -o "$TARGET"
chmod +x "$TARGET"

echo "==> Installed to $TARGET"

case ":$PATH:" in
    *":$INSTALL_DIR:"*)
        echo "==> $INSTALL_DIR is already on your PATH."
        ;;
    *)
        echo ""
        echo "==> $INSTALL_DIR is not on your PATH yet. Add this to your shell profile"
        echo "    (~/.bashrc, ~/.zshrc, ~/.profile, etc.) and restart your shell:"
        echo ""
        echo "    export PATH=\"$INSTALL_DIR:\$PATH\""
        echo ""
        ;;
esac

echo "==> Run 'mecseksat help' to get started."
