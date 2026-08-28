#!/usr/bin/env bash
# Build Auxilius installers
#
# Linux  → bin/auxilius-installer     (assistente gráfico GTK3 via wxWidgets)
#         dist/auxilius_0.1.0_amd64.deb  (pacote instalável/desinstalável via dpkg/apt)
# Windows → dist/auxilius-setup-0.1.0.exe  (NSIS — registra no Painel de Controle)
#
# Uso:
#   bash installer/build.sh              # detecta plataforma
#   bash installer/build.sh linux        # GTK3 wizard + .deb
#   bash installer/build.sh linux-deb    # apenas .deb
#   bash installer/build.sh windows      # apenas .exe (requer makensis)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PLATFORM="${1:-$(uname -s | tr '[:upper:]' '[:lower:]')}"

build_gtk_wizard() {
    OUT="$ROOT/bin/auxilius-installer"
    echo "› Compilando assistente gráfico wxWidgets (Linux GTK3)..."
    g++ -std=c++20 -O2 \
        $(wx-config --cxxflags) \
        "$ROOT/installer/src/Installer.cpp" \
        $(wx-config --libs) \
        -lstdc++fs \
        -o "$OUT"
    echo "✓ Assistente: $OUT"
    echo "  Execute:    $OUT"
}

build_deb() {
    echo "› Gerando pacote .deb (Debian/Ubuntu)..."
    bash "$ROOT/installer/linux/build-deb.sh"
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
    echo "  Registra e desinstala pelo Painel de Controle > Programas"
}

case "$PLATFORM" in
    linux)     build_gtk_wizard; build_deb ;;
    linux-deb) build_deb ;;
    windows)   build_windows ;;
    *)         build_gtk_wizard; build_deb ;;
esac


