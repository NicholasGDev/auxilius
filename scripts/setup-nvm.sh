#!/usr/bin/env bash
# Instala NVM e configura Node 19 e 20 (padrão: 20)

set -euo pipefail

export NVM_DIR="${NVM_DIR:-$HOME/.nvm}"

echo "Instalando NVM v0.39.7…"
if [ ! -s "$NVM_DIR/nvm.sh" ]; then
  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
fi

# shellcheck disable=SC1091
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"

echo "Instalando Node 19…"
nvm install 19

echo "Instalando Node 20…"
nvm install 20

echo "Definindo Node 20 como padrão…"
nvm alias default 20
nvm use 20

echo ""
echo "✓ Node $(node -v) configurado como padrão"
echo "✓ Versões disponíveis:"
nvm ls
