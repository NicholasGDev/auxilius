#include "Scaffold.hpp"
#include "../../shared/FileSystem.hpp"
#include <string>

namespace Infra {

using Shared::writeFile;
using Shared::mkdirs;
namespace fs = std::filesystem;

void generateElectronScaffold(const fs::path& root)
{
    const fs::path r   = root / "electron";
    const fs::path src = r / "src";

    mkdirs({
        src / "main" / "ipc",
        src / "preload",
        src / "renderer" / "src" / "app",
        src / "renderer" / "src" / "components" / "common",
        src / "renderer" / "src" / "components" / "layout",
        src / "renderer" / "src" / "pages" / "Home",
        src / "renderer" / "src" / "routes",
        src / "renderer" / "src" / "styles",
        src / "renderer" / "src" / "types",
    });

    writeFile(r / "package.json", R"({
  "name": "zeus-retail-electron",
  "version": "0.1.0",
  "description": "Zeus Retail Evolution — Electron desktop wrapper",
  "main": "out/main/index.js",
  "scripts": {
    "dev": "electron-vite dev",
    "build": "electron-vite build",
    "preview": "electron-vite preview",
    "package": "electron-vite build && electron-builder"
  },
  "dependencies": {
    "@giro-ds/react": "^9.0.0",
    "@giro-ds/tokens": "^1.0.1",
    "@giro-ds/utilities": "^1.1.0",
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "react-router-dom": "^6.18.0"
  },
  "devDependencies": {
    "@types/node": "^20.0.0",
    "@types/react": "^18.2.0",
    "@types/react-dom": "^18.2.0",
    "@vitejs/plugin-react": "^4.2.0",
    "electron": "^28.0.0",
    "electron-builder": "^24.0.0",
    "electron-vite": "^2.0.0",
    "sass": "^1.69.0",
    "typescript": "^5.2.0",
    "vite": "^5.0.0"
  },
  "build": {
    "appId": "com.zanthus.zeus-retail",
    "productName": "Zeus Retail Evolution",
    "linux": { "target": ["AppImage", "deb"] },
    "win":   { "target": ["nsis"] }
  }
})");

    writeFile(r / ".gitignore", R"(node_modules/
out/
dist/
release/
*.AppImage
*.deb
*.dmg
*.exe
*.snap
.env
.env.local
*.local
)");

    writeFile(r / "electron.vite.config.ts", R"(import { resolve } from 'path'
import { defineConfig, externalizeDepsPlugin } from 'electron-vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  main:     { plugins: [externalizeDepsPlugin()] },
  preload:  { plugins: [externalizeDepsPlugin()] },
  renderer: {
    resolve: { alias: { '@renderer': resolve('src/renderer/src') } },
    plugins: [react()]
  }
})
)");

    writeFile(r / "tsconfig.json", R"({
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "module": "CommonJS",
    "moduleResolution": "node",
    "strict": true,
    "jsx": "react-jsx",
    "skipLibCheck": true,
    "esModuleInterop": true,
    "resolveJsonModule": true,
    "noEmit": true,
    "paths": { "@renderer/*": ["./src/renderer/src/*"] }
  },
  "include": ["electron.vite.config.ts", "src/**/*.ts", "src/**/*.tsx"],
  "exclude": ["node_modules", "out", "dist"]
}
)");

    writeFile(src / "main" / "index.ts", R"(import { app, BrowserWindow } from 'electron'
import { join } from 'path'

const isDev = process.env.NODE_ENV === 'development'

function createWindow(): void {
  const win = new BrowserWindow({
    width: 1280,
    height: 800,
    show: false,
    autoHideMenuBar: true,
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false,
      contextIsolation: true,
      nodeIntegration: false
    }
  })
  win.on('ready-to-show', () => win.show())
  if (isDev && process.env['ELECTRON_RENDERER_URL']) {
    win.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    win.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

app.whenReady().then(() => {
  createWindow()
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})
app.on('window-all-closed', () => { if (process.platform !== 'darwin') app.quit() })
)");

    writeFile(src / "preload" / "index.ts", R"(import { contextBridge } from 'electron'

contextBridge.exposeInMainWorld('zeusApi', {
  version: () => process.versions.electron
})
)");

    writeFile(src / "renderer" / "index.html", R"(<!DOCTYPE html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Zeus Retail Evolution</title>
  </head>
  <body>
    <div id="root"></div>
    <script type="module" src="/src/main.tsx"></script>
  </body>
</html>
)");

    writeFile(src / "renderer" / "src" / "main.tsx", R"(import React from 'react'
import ReactDOM from 'react-dom/client'
import '@giro-ds/tokens/build/css/tokens.css'
import '@giro-ds/react/dist/styles.css'
import './styles/global.scss'
import App from './App'

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode><App /></React.StrictMode>
)
)");

    writeFile(src / "renderer" / "src" / "App.tsx", R"(import React from 'react'
import { HashRouter, Routes, Route } from 'react-router-dom'
import { HomePage } from './pages/Home'

const App: React.FC = () => (
  <HashRouter>
    <Routes>
      <Route path="/" element={<HomePage />} />
    </Routes>
  </HashRouter>
)
export default App
)");

    writeFile(src / "renderer" / "src" / "pages" / "Home" / "index.tsx", R"(import React from 'react'
import { Button } from '@giro-ds/react'

export const HomePage: React.FC = () => (
  <div style={{ padding: 32 }}>
    <h1>Zeus Retail Evolution</h1>
    <p style={{ margin: '8px 0 24px' }}>Electron + React + Giro DS</p>
    <Button variant="filled">Acessar PDV</Button>
  </div>
)
)");

    writeFile(src / "renderer" / "src" / "styles" / "global.scss",
        "* { box-sizing: border-box; margin: 0; padding: 0; }\n"
        "body {\n"
        "  font-family: var(--font-family-primary, 'Figtree', sans-serif);\n"
        "  background-color: var(--color-neutral-high-light, #f5f5f5);\n"
        "}\n");
}

} // namespace Infra
