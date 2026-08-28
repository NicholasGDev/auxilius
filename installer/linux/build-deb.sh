#!/usr/bin/env bash
# Build a .deb package for Auxilius
# Install:   dpkg -i dist/auxilius_0.1.0_amd64.deb
# Uninstall: dpkg -r auxilius   OR   apt remove auxilius
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
PKG_NAME="auxilius"
PKG_VER="0.1.0"
PKG_ARCH="amd64"
PKG_DIR="$ROOT/dist/deb/${PKG_NAME}_${PKG_VER}_${PKG_ARCH}"
OUT_DEB="$ROOT/dist/${PKG_NAME}_${PKG_VER}_${PKG_ARCH}.deb"

# ── Binary must exist ─────────────────────────────────────────────────────────
if [ ! -f "$ROOT/bin/scaffold_zeus" ]; then
  echo "✗ Compile o binário antes: npm run cpp:compile"
  exit 1
fi

rm -rf "$PKG_DIR"

# ── Filesystem layout ─────────────────────────────────────────────────────────
install -d "$PKG_DIR/DEBIAN"
install -d "$PKG_DIR/opt/auxilius"
install -d "$PKG_DIR/opt/auxilius/bin"
install -d "$PKG_DIR/usr/bin"
install -d "$PKG_DIR/usr/share/applications"
install -d "$PKG_DIR/usr/share/doc/$PKG_NAME"

# Copy entire app source into /opt/auxilius (node_modules excluded — installed by postinst)
rsync -a --exclude='node_modules' --exclude='.git' --exclude='dist' --exclude='dist' \
    "$ROOT/" "$PKG_DIR/opt/auxilius/"

# scaffold_zeus binary
install -m 755 "$ROOT/bin/scaffold_zeus" "$PKG_DIR/opt/auxilius/bin/scaffold_zeus"
ln -sf "/opt/auxilius/bin/scaffold_zeus" "$PKG_DIR/usr/bin/scaffold_zeus"

# Launcher script — sources nvm then runs electron-vite dev
cat > "$PKG_DIR/usr/bin/auxilius" << 'LAUNCHER'
#!/usr/bin/env bash
export NVM_DIR="${NVM_DIR:-$HOME/.nvm}"
[ -s "$NVM_DIR/nvm.sh" ] && source "$NVM_DIR/nvm.sh"
cd /opt/auxilius
exec npm run dev -- --no-sandbox "$@"
LAUNCHER
chmod 755 "$PKG_DIR/usr/bin/auxilius"

# .desktop entry
cat > "$PKG_DIR/usr/share/applications/auxilius.desktop" << DESKTOP
[Desktop Entry]
Name=Auxilius
GenericName=Developer Toolbox
Comment=Zeus Retail Evolution — Scaffold, Endpoints e Ambiente
Exec=/usr/bin/auxilius
Icon=/opt/auxilius/resources/icon.png
Terminal=false
Type=Application
Categories=Development;IDE;
Keywords=zeus;scaffold;php;laravel;ddd;
DESKTOP

# changelog
echo "$PKG_NAME ($PKG_VER) unstable; urgency=low" > /tmp/changelog
echo "  * Versão inicial." >> /tmp/changelog
gzip -9 -c /tmp/changelog > "$PKG_DIR/usr/share/doc/$PKG_NAME/changelog.gz"

# ── DEBIAN/control ────────────────────────────────────────────────────────────
cat > "$PKG_DIR/DEBIAN/control" << CTRL
Package: $PKG_NAME
Version: $PKG_VER
Architecture: $PKG_ARCH
Maintainer: Zanthus Tecnologia <dev@zanthus.com.br>
Depends: libsqlite3-0, git, curl, build-essential
Recommends: nodejs (>= 20)
Section: devel
Priority: optional
Description: Zeus Retail Evolution — Developer Toolbox
 Electron + React + Giro DS com motor C++20 DDD.
 Scaffold Laravel DDD, gerador de endpoints PHP, layout VS Code.
 Persiste configurações via SQLite (C++ nativo).
CTRL

# ── DEBIAN/postinst ───────────────────────────────────────────────────────────
cat > "$PKG_DIR/DEBIAN/postinst" << 'POST'
#!/usr/bin/env bash
set -e
cd /opt/auxilius

# Install nvm + Node 20 for the installing user
REAL_USER="${SUDO_USER:-$(logname 2>/dev/null || echo $USER)}"
run_as_user() {
  if [ "$REAL_USER" != "root" ] && command -v sudo &>/dev/null; then
    sudo -u "$REAL_USER" bash -c "$1"
  else
    bash -c "$1"
  fi
}

run_as_user '
  export NVM_DIR="${NVM_DIR:-$HOME/.nvm}"
  [ -d "$NVM_DIR" ] || curl -fsSL https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash
  source "$NVM_DIR/nvm.sh"
  nvm install 20 --silent
  nvm use 20
  cd /opt/auxilius && npm install --prefer-offline --silent
'
echo "Auxilius instalado. Execute: auxilius"
POST
chmod 755 "$PKG_DIR/DEBIAN/postinst"

# ── DEBIAN/prerm ──────────────────────────────────────────────────────────────
cat > "$PKG_DIR/DEBIAN/prerm" << 'PRERM'
#!/usr/bin/env bash
set -e
# Kill any running instance before removal
pkill -f "auxilius" 2>/dev/null || true
echo "Removendo Auxilius..."
PRERM
chmod 755 "$PKG_DIR/DEBIAN/prerm"

# ── DEBIAN/postrm ─────────────────────────────────────────────────────────────
cat > "$PKG_DIR/DEBIAN/postrm" << 'POSTRM'
#!/usr/bin/env bash
set -e
if [ "$1" = "purge" ]; then
  rm -rf /opt/auxilius
  echo "Arquivos de configuração em ~/.auxilius mantidos. Remova manualmente se desejar."
fi
POSTRM
chmod 755 "$PKG_DIR/DEBIAN/postrm"

# ── md5sums ───────────────────────────────────────────────────────────────────
find "$PKG_DIR" -not -path "*/DEBIAN/*" -type f \
  | xargs md5sum 2>/dev/null \
  | sed "s|$PKG_DIR/||" \
  > "$PKG_DIR/DEBIAN/md5sums"

# ── Build .deb ────────────────────────────────────────────────────────────────
mkdir -p "$ROOT/dist"
dpkg-deb --build --root-owner-group "$PKG_DIR" "$OUT_DEB"

echo ""
echo "✓ Pacote .deb gerado: $OUT_DEB"
echo ""
echo "  Instalar:    sudo dpkg -i $OUT_DEB"
echo "  Desinstalar: sudo dpkg -r $PKG_NAME"
echo "  Purgar:      sudo dpkg -P $PKG_NAME  (remove também /opt/auxilius)"
echo "  Via apt:     sudo apt remove $PKG_NAME"
