#!/usr/bin/env bash
# Auxilius — Universal Installer
# Supports: Ubuntu/Debian Linux, macOS (Homebrew)
set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'
ok()   { echo -e "${GREEN}✓${NC} $*"; }
info() { echo -e "${BLUE}›${NC} $*"; }
warn() { echo -e "${YELLOW}⚠${NC} $*"; }
fail() { echo -e "${RED}✗${NC} $*"; exit 1; }

OS="$(uname -s)"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo ""
echo -e "${BLUE}╔══════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  Auxilius — Universal Installer      ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════╝${NC}"
echo ""

# ── 1. Install system deps ────────────────────────────────────────────────────
info "Detectando sistema operacional: $OS"

install_linux() {
  if ! command -v apt-get &>/dev/null; then
    warn "apt-get não encontrado — instale manualmente: g++ libsqlite3-dev"
    return
  fi
  info "Instalando dependências do sistema (apt)..."
  sudo apt-get update -qq
  sudo apt-get install -y -qq build-essential g++ libsqlite3-dev git curl
  ok "Dependências do sistema instaladas"
}

install_macos() {
  if ! command -v brew &>/dev/null; then
    info "Instalando Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  fi
  info "Instalando dependências (brew)..."
  brew install gcc sqlite git curl || true
  ok "Dependências do sistema instaladas"
}

case "$OS" in
  Linux)  install_linux ;;
  Darwin) install_macos ;;
  *)      warn "Sistema não reconhecido: $OS — pule deps manuais se necessário" ;;
esac

# ── 2. Node.js via nvm ────────────────────────────────────────────────────────
NVM_DIR="${NVM_DIR:-$HOME/.nvm}"

if [ ! -d "$NVM_DIR" ]; then
  info "Instalando nvm..."
  curl -fsSL https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash
fi

export NVM_DIR
# shellcheck source=/dev/null
[ -s "$NVM_DIR/nvm.sh" ] && source "$NVM_DIR/nvm.sh"

if ! command -v nvm &>/dev/null; then
  fail "nvm não encontrado após instalação. Reinicie o terminal e execute: bash scripts/install.sh"
fi

info "Instalando Node.js 20 LTS..."
nvm install 20
nvm use 20
nvm alias default 20
ok "Node.js $(node -v) ativo"

# ── 3. npm install ────────────────────────────────────────────────────────────
info "Instalando dependências npm..."
cd "$ROOT_DIR"
npm install --prefer-offline 2>&1 | tail -3
ok "npm install concluído"

# ── 4. Compile C++ binary ────────────────────────────────────────────────────
info "Compilando binário C++ (scaffold_zeus)..."
npm run cpp:compile
ok "Binário compilado: $ROOT_DIR/bin/scaffold_zeus"

# ── 5. Verify SQLite DB path ──────────────────────────────────────────────────
DB_DIR="$HOME/.auxilius"
mkdir -p "$DB_DIR"
info "Banco SQLite em: $DB_DIR/auxilius.db"
"$ROOT_DIR/bin/scaffold_zeus" db set installedAt "$(date -Iseconds)" >/dev/null
ok "SQLite funcionando"

# ── 6. Desktop entry (Linux only) ────────────────────────────────────────────
if [ "$OS" = "Linux" ] && [ -d "$HOME/.local/share/applications" ]; then
  info "Criando atalho do desktop..."
  cat > "$HOME/.local/share/applications/auxilius.desktop" << DESKTOP
[Desktop Entry]
Name=Auxilius
Comment=Zeus Retail Evolution Developer Toolbox
Exec=bash -c 'cd "$ROOT_DIR" && npm run dev'
Icon=$ROOT_DIR/resources/icon.png
Terminal=false
Type=Application
Categories=Development;
DESKTOP
  ok "Atalho criado em ~/.local/share/applications/auxilius.desktop"
fi

# ── 7. Summary ────────────────────────────────────────────────────────────────
echo ""
echo -e "${GREEN}╔══════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  Instalação concluída!               ║${NC}"
echo -e "${GREEN}╚══════════════════════════════════════╝${NC}"
echo ""
echo "  Para iniciar:    npm run dev"
echo "  Para compilar:   npm run cpp:compile"
echo "  Banco de dados:  $DB_DIR/auxilius.db"
echo ""
