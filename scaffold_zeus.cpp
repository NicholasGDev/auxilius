#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static void createFile(const fs::path& path, const std::string& content)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path);
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

static void mkdirs(std::initializer_list<fs::path> dirs)
{
    for (const auto& d : dirs) fs::create_directories(d);
}

// ═══════════════════════════════════════════════════════════════════════════════
// [1] BACKEND  →  <root>/back/
// ═══════════════════════════════════════════════════════════════════════════════

static void generateBackendScaffold(const fs::path& root)
{
    const fs::path ctxBase = root / "back" / "app" / "Contexts";

    const std::vector<std::string> contexts = {
        "Autenticacao", "Clientes", "Compartilhado", "Devolucoes", "Lojas",
        "Mercadorias", "Promocoes", "Vendas", "Tesouraria", "Finalizadoras",
        "Funcionarios", "Pdvs", "Combos", "Documentacao"
    };

    for (const auto& ctx : contexts) {
        const fs::path c = ctxBase / ctx;
        mkdirs({
            c / "Application" / "DTOs" / "Inputs",
            c / "Application" / "DTOs" / "Outputs",
            c / "Application" / "Errors",
            c / "Application" / "Exceptions",
            c / "Application" / "Queries",
            c / "Application" / "UseCases",
            c / "Application" / "Services",
            c / "Domain" / "Entities",
            c / "Domain" / "Enums",
            c / "Domain" / "Filters",
            c / "Domain" / "Autorizacoes",
            c / "Infra" / "Persistence" / "Models",
            c / "Infra" / "Persistence" / "Repositories",
            c / "Infra" / "Presentation" / "Http" / "Controllers",
            c / "Infra" / "Presentation" / "Http" / "Requests",
            c / "Infra" / "Presentation" / "Routes",
            c / "Infra" / "Providers",
        });

        createFile(c / "Infra" / "Providers" / (ctx + "ServiceProvider.php"),
            "<?php\n\ndeclare(strict_types=1);\n\n"
            "namespace App\\Contexts\\" + ctx + "\\Infra\\Providers;\n\n"
            "use App\\Contexts\\Compartilhado\\Base\\Infra\\Providers\\CompartilhadoServiceProvider;\n"
            "use Illuminate\\Foundation\\Application;\n\n"
            "class " + ctx + "ServiceProvider extends CompartilhadoServiceProvider\n{\n"
            "    public function __construct(Application $app)\n    {\n"
            "        parent::__construct($app);\n"
            "        $this->setPrefix('" + ctx + "');\n"
            "        $this->setName('" + ctx + "');\n"
            "        $this->setRoute(__DIR__ . '/../Presentation/Routes/api.php');\n"
            "    }\n}\n");

        createFile(c / "Infra" / "Presentation" / "Routes" / "api.php",
            "<?php\n\ndeclare(strict_types=1);\n\n"
            "use Illuminate\\Support\\Facades\\Route;\n\n"
            "Route::get('/consultar', fn() => response()->json(['status' => 'OK', 'context' => '" + ctx + "']));\n");
    }

    // Registers all context ServiceProviders
    std::string providers = "<?php\n\nreturn [\n";
    for (const auto& ctx : contexts)
        providers += "    App\\Contexts\\" + ctx + "\\Infra\\Providers\\" + ctx + "ServiceProvider::class,\n";
    providers += "];\n";
    createFile(root / "back" / "bootstrap" / "providers.php", providers);

    createFile(root / "back" / ".gitignore", R"(/vendor/
/.env
/.env.*
!/.env.example
/storage/*.key
/node_modules/
/public/hot
/public/storage
/storage/app/public
/storage/framework/cache/data
/storage/framework/sessions
/storage/framework/views
/storage/logs
Homestead.json
Homestead.yaml
auth.json
npm-debug.log
/.fleet
/.idea
/.vscode
)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// [2] FRONTEND  →  <root>/front/
// ═══════════════════════════════════════════════════════════════════════════════

static void generateFrontendScaffold(const fs::path& root)
{
    const fs::path r   = root / "front";
    const fs::path src = r / "src";

    mkdirs({
        src / "app",
        src / "components" / "common",
        src / "components" / "layout",
        src / "hooks",
        src / "layouts",
        src / "pages" / "Dashboard" / "components",
        src / "pages" / "Vendas" / "components",
        src / "pages" / "Tesouraria" / "components",
        src / "routes",
        src / "store" / "slices",
        src / "styles",
        src / "types",
        src / "utils",
    });

    // package.json — mirrors production front project (same Giro DS versions)
    createFile(r / "package.json", R"({
  "name": "zeus-retail-front",
  "version": "0.1.0",
  "private": true,
  "scripts": {
    "start": "vite",
    "build": "vite build",
    "test": "vitest",
    "preview": "vite preview"
  },
  "dependencies": {
    "@giro-ds/react": "^9.0.0",
    "@giro-ds/tokens": "^1.0.1",
    "@giro-ds/utilities": "^1.1.0",
    "@hookform/resolvers": "^5.2.2",
    "@reduxjs/toolkit": "^2.9.0",
    "axios": "^1.6.1",
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "react-hook-form": "^7.62.0",
    "react-redux": "^8.1.3",
    "react-router-dom": "^6.18.0",
    "react-toastify": "^9.1.3",
    "redux": "^4.2.1",
    "redux-persist": "^6.0.0",
    "sass": "^1.79.4",
    "yup": "^1.7.0"
  },
  "devDependencies": {
    "@types/node": "^24.0.0",
    "@types/react": "^18.3.0",
    "@types/react-dom": "^18.3.0",
    "@vitejs/plugin-react-swc": "^4.1.0",
    "typescript": "^4.9.5",
    "vite": "^7.1.7",
    "vitest": "^3.2.4"
  }
})");

    createFile(r / "vite.config.ts", R"(import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react-swc'
import { resolve } from 'path'

export default defineConfig({
  plugins: [react()],
  resolve: { alias: { '@': resolve(__dirname, 'src') } }
})
)");

    createFile(r / "tsconfig.json", R"({
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "module": "ESNext",
    "moduleResolution": "bundler",
    "strict": true,
    "jsx": "react-jsx",
    "skipLibCheck": true,
    "esModuleInterop": true,
    "resolveJsonModule": true,
    "isolatedModules": true,
    "noEmit": true,
    "baseUrl": ".",
    "paths": { "@/*": ["src/*"] }
  },
  "include": ["src"],
  "exclude": ["node_modules", "dist"]
}
)");

    createFile(r / "index.html", R"(<!DOCTYPE html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Zeus Retail Evolution</title>
  </head>
  <body>
    <div id="root"></div>
    <script type="module" src="/src/app/main.tsx"></script>
  </body>
</html>
)");

    createFile(r / ".gitignore", "node_modules/\ndist/\n.env\n.env.local\n*.local\n");

    createFile(src / "app" / "main.tsx", R"(import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider } from 'react-redux';
import { store } from '../store';
import App from './App';
import '@giro-ds/tokens/build/css/tokens.css';
import '@giro-ds/react/dist/styles.css';
import '../styles/global.scss';

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <Provider store={store}>
      <App />
    </Provider>
  </React.StrictMode>
);
)");

    createFile(src / "app" / "App.tsx", R"(import React from 'react';
import { AppRoutes } from '../routes';

const App: React.FC = () => <AppRoutes />;
export default App;
)");

    createFile(src / "routes" / "index.tsx", R"(import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { DashboardPage } from '../pages/Dashboard';

export const AppRoutes: React.FC = () => (
  <BrowserRouter>
    <Routes>
      <Route path="/" element={<DashboardPage />} />
    </Routes>
  </BrowserRouter>
);
)");

    createFile(src / "store" / "index.ts", R"(import { configureStore } from '@reduxjs/toolkit';

export const store = configureStore({ reducer: {} });

export type RootState   = ReturnType<typeof store.getState>;
export type AppDispatch = typeof store.dispatch;
)");

    createFile(src / "styles" / "global.scss", R"(* { box-sizing: border-box; margin: 0; padding: 0; }
body { font-family: var(--font-family-primary, 'Figtree', sans-serif); }
)");

    createFile(src / "components" / "common" / "HeaderGlobal.tsx", R"(import React from 'react';
import { Button } from '@giro-ds/react';

export const HeaderGlobal: React.FC = () => (
  <header className="header-global">
    <h2>Zeus Retail Evolution</h2>
    <Button variant="filled">PDV Online</Button>
  </header>
);
)");

    createFile(src / "pages" / "Dashboard" / "index.tsx", R"(import React from 'react';
import { DashboardCard } from './components/DashboardCard';

export const DashboardPage: React.FC = () => (
  <div className="dashboard-page">
    <h1>Dashboard — Zeus Retail</h1>
    <DashboardCard title="Vendas do Dia" value="R$ 12.450,00" />
  </div>
);
)");

    // Co-location: component lives beside the page that owns it
    createFile(src / "pages" / "Dashboard" / "components" / "DashboardCard.tsx", R"(import React from 'react';

interface Props { title: string; value: string; }

export const DashboardCard: React.FC<Props> = ({ title, value }) => (
  <div className="dashboard-card">
    <h3>{title}</h3>
    <p>{value}</p>
  </div>
);
)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// [3] ELECTRON  →  <root>/electron/
// ═══════════════════════════════════════════════════════════════════════════════

static void generateElectronScaffold(const fs::path& root)
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

    createFile(r / "package.json", R"({
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

    createFile(r / ".gitignore", R"(node_modules/
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

    createFile(r / "electron.vite.config.ts", R"(import { resolve } from 'path'
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

    createFile(r / "tsconfig.json", R"({
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "module": "ESNext",
    "moduleResolution": "bundler",
    "strict": true,
    "jsx": "react-jsx",
    "skipLibCheck": true,
    "esModuleInterop": true,
    "resolveJsonModule": true,
    "isolatedModules": true,
    "noEmit": true,
    "paths": { "@renderer/*": ["./src/renderer/src/*"] }
  },
  "include": ["electron.vite.config.ts", "src/**/*.ts", "src/**/*.tsx"],
  "exclude": ["node_modules", "out", "dist"]
}
)");

    createFile(src / "main" / "index.ts", R"(import { app, BrowserWindow } from 'electron'
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

    createFile(src / "preload" / "index.ts", R"(import { contextBridge } from 'electron'

contextBridge.exposeInMainWorld('zeusApi', {
  version: () => process.versions.electron
})
)");

    createFile(src / "renderer" / "index.html", R"(<!DOCTYPE html>
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

    createFile(src / "renderer" / "src" / "main.tsx", R"(import React from 'react'
import ReactDOM from 'react-dom/client'
import '@giro-ds/tokens/build/css/tokens.css'
import '@giro-ds/react/dist/styles.css'
import './styles/global.scss'
import App from './App'

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode><App /></React.StrictMode>
)
)");

    createFile(src / "renderer" / "src" / "App.tsx", R"(import React from 'react'
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

    createFile(src / "renderer" / "src" / "pages" / "Home" / "index.tsx", R"(import React from 'react'
import { Button } from '@giro-ds/react'

export const HomePage: React.FC = () => (
  <div style={{ padding: 32 }}>
    <h1>Zeus Retail Evolution</h1>
    <p style={{ margin: '8px 0 24px' }}>Electron + React + Giro DS</p>
    <Button variant="filled">Acessar PDV</Button>
  </div>
)
)");

    createFile(src / "renderer" / "src" / "styles" / "global.scss", R"(* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  font-family: var(--font-family-primary, 'Figtree', sans-serif);
  background-color: var(--color-neutral-high-light, #f5f5f5);
}
)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// main — generates all three scaffolds into separate directories
// ═══════════════════════════════════════════════════════════════════════════════

int main(int argc, char* argv[])
{
    fs::path projectRoot = fs::current_path() / "ZeusRetailEvolution";
    if (argc > 1) projectRoot = fs::path(argv[1]);

    std::cout << "Gerando estrutura do Zeus Retail Evolution em: " << projectRoot << "\n\n";

    try {
        std::cout << "  [1/3] back/     — Laravel DDD (14 contextos)\n";
        generateBackendScaffold(projectRoot);

        std::cout << "  [2/3] front/    — React + Giro DS (Vite)\n";
        generateFrontendScaffold(projectRoot);

        std::cout << "  [3/3] electron/ — Electron + React + Giro DS\n";
        generateElectronScaffold(projectRoot);

        // Root .gitignore covering all three sub-projects
        createFile(projectRoot / ".gitignore", R"(# Backend
back/vendor/
back/.env
back/.env.*
!back/.env.example
back/storage/*.key

# Frontend
front/node_modules/
front/dist/

# Electron
electron/node_modules/
electron/out/
electron/dist/
electron/release/

# Common
*.local
.DS_Store
Thumbs.db
)");

        std::cout << "\nScaffold gerado com sucesso!\n"
                  << "  " << (projectRoot / "back").string()     << "\n"
                  << "  " << (projectRoot / "front").string()    << "\n"
                  << "  " << (projectRoot / "electron").string() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

