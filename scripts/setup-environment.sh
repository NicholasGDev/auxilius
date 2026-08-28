#!/usr/bin/env bash
# Full environment setup for Zeus Retail Evolution development
# Run as root inside WSL Ubuntu 22

set -euo pipefail

echo "========================================"
echo " Auxilius — Setup Ambiente Zeus Retail"
echo "========================================"

# ── 1. WSL default user → root ────────────────────────────────────────────────
WSL_CONF="/etc/wsl.conf"
echo "[1/4] Configurando WSL usuário padrão → root"
if ! grep -q '\[user\]' "$WSL_CONF" 2>/dev/null; then
  cat >> "$WSL_CONF" <<'EOF'

[user]
default=root
EOF
  echo "  ✓ /etc/wsl.conf atualizado. Reinicie o WSL: wsl --shutdown"
else
  echo "  ~ Já configurado"
fi

# ── 2. Git ────────────────────────────────────────────────────────────────────
echo "[2/4] Verificando Git"
if ! command -v git &>/dev/null; then
  apt-get update -qq && apt-get install -y git
fi
echo "  ✓ $(git --version)"

# ── 3. NVM + Node 20 (padrão) e 19 ───────────────────────────────────────────
echo "[3/4] Instalando NVM, Node 19 e 20"
export NVM_DIR="$HOME/.nvm"

if [ ! -s "$NVM_DIR/nvm.sh" ]; then
  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
fi

# Source NVM in current shell
# shellcheck disable=SC1091
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"

nvm install 19
nvm install 20
nvm alias default 20
nvm use 20

echo "  ✓ Node $(node -v) | npm $(npm -v)"

# Add NVM sourcing to .bashrc if not present
if ! grep -q 'NVM_DIR' ~/.bashrc; then
  cat >> ~/.bashrc <<'EOF'

# NVM
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
[ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"
EOF
fi

# ── 4. Clone project ──────────────────────────────────────────────────────────
echo "[4/4] Clonando Zeus Retail Evolution"
PROJECT_DIR="$HOME/projects/zeus-retail-evolution"
mkdir -p "$HOME/projects"

if [ -d "$PROJECT_DIR" ]; then
  echo "  ~ Projeto já existe em $PROJECT_DIR"
else
  git clone https://gitlab.zanthus.com.br/web/zeus-retail-evolution.git "$PROJECT_DIR"
  echo "  ✓ Clonado em $PROJECT_DIR"
fi

echo ""
echo "========================================"
echo " Setup concluído!"
echo " Node: $(node -v) | NVM: $(nvm --version)"
echo " Projeto: $PROJECT_DIR"
echo "========================================"
