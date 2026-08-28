#!/usr/bin/env bash
# Build the Auxilius installer
#   Linux  → bin/auxilius-installer  (GTK3 wizard via wxWidgets)
#   Windows → dist/auxilius-setup-0.1.0.exe  (NSIS — requer makensis instalado)
#
# Uso:
#   bash installer/build.sh           # detecta plataforma
#   bash installer/build.sh linux
#   bash installer/build.sh windows   # gera .exe via NSIS (requer makensis)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PLATFORM="${1:-$(uname -s | tr '[:upper:]' '[:lower:]')}"

build_linux() {
    OUT="$ROOT/bin/auxilius-installer"
    echo "› Compilando instalador wxWidgets (Linux)..."
    g++ -std=c++20 -O2 \
        $(wx-config --cxxflags) \
        "$ROOT/installer/src/Installer.cpp" \
        $(wx-config --libs) \
        -lstdc++fs \
        -o "$OUT"
    echo "✓ Instalador Linux: $OUT"
    echo "  Execute: $OUT"
}

build_windows() {
    if ! command -v makensis &>/dev/null; then
        echo "✗ makensis não encontrado."
        echo "  Linux/WSL: sudo apt-get install nsis"
        echo "  Windows:   https://nsis.sourceforge.io/Download"
        exit 1
    fi
    mkdir -p "$ROOT/dist"
    echo "› Gerando instalador Windows (.exe) com NSIS..."
    makensis "$ROOT/installer/windows/auxilius.nsi"
    echo "✓ Instalador Windows: $ROOT/dist/auxilius-setup-0.1.0.exe"
    echo "  O .exe registra/desinstala pelo Painel de Controle > Programas do Windows"
}

case "$PLATFORM" in
    linux)   build_linux   ;;
    windows) build_windows ;;
    *)       build_linux   ;;  # padrão em outros sistemas
esac

