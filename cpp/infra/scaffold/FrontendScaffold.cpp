#include "Scaffold.hpp"
#include "../../shared/FileSystem.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace Infra {

using Shared::writeFile;
using Shared::mkdirs;
namespace fs = std::filesystem;

static std::string toLower(const std::string& s)
{
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return r;
}

void generateFrontendScaffold(const fs::path& root)
{
    const fs::path r   = root / "front";
    const fs::path src = r / "src";

    const std::vector<std::string> contexts = {
        "Autenticacao", "Clientes", "Devolucoes", "Lojas",
        "Mercadorias", "Promocoes", "Vendas", "Tesouraria", "Finalizadoras",
        "Funcionarios", "Pdvs", "Combos", "Documentacao"
    };

    // Sub-pages per context (matching Treasury pattern)
    const std::vector<std::string> subPages = {
        "Consultar", "Criar", "Detalhar"
    };

    mkdirs({
        src / "app",
        src / "components" / "common",
        src / "components" / "layout",
        src / "hooks",
        src / "layouts",
        src / "routes",
        src / "store" / "slices",
        src / "styles",
        src / "types",
        src / "utils",
    });

    for (const auto& ctx : contexts) {
        const fs::path pageDir = src / "app" / "pages" / ctx;
        fs::create_directories(pageDir);
        for (const auto& sub : subPages)
            fs::create_directories(pageDir / sub);
    }

    // ── Per-context page files ────────────────────────────────────────────────
    for (const auto& ctx : contexts) {
        const fs::path pageDir = src / "app" / "pages" / ctx;
        const std::string ctxLower = toLower(ctx);

        // Root index.tsx
        writeFile(pageDir / "index.tsx",
            "import React from 'react';\n"
            "import styles from './index.module.scss';\n\n"
            "const " + ctx + "Page: React.FC = () => (\n"
            "  <div className={styles.page}>\n"
            "    <h1>" + ctx + "</h1>\n"
            "  </div>\n"
            ");\n\n"
            "export default " + ctx + "Page;\n");

        // Root index.module.scss
        writeFile(pageDir / "index.module.scss",
            ".page {\n  padding: 1.5rem;\n}\n");

        // Routes file
        std::string routesImports = "import " + ctx + "Page from '.';\n";
        std::string routesArr;
        for (const auto& sub : subPages) {
            const std::string subLower = toLower(sub);
            routesImports += "import " + sub + ctx + " from './" + sub + "';\n";
            routesArr += "  { path: '/" + ctxLower + "/" + subLower + "', component: " + sub + ctx + ", permission: '" + ctxLower + "/" + subLower + "' },\n";
        }
        writeFile(pageDir / (ctxLower + ".routes.ts"),
            routesImports + "\nexport const " + ctxLower + "Routes = [\n"
            "  { path: '/" + ctxLower + "', component: " + ctx + "Page, permission: '" + ctxLower + "' },\n"
            + routesArr + "];\n");

        // Sub-pages
        for (const auto& sub : subPages) {
            const fs::path subDir = pageDir / sub;

            writeFile(subDir / "index.tsx",
                "import React from 'react';\n"
                "import styles from './index.module.scss';\n\n"
                "const " + sub + ctx + ": React.FC = () => (\n"
                "  <div className={styles.page}>\n"
                "    <h2>" + sub + " — " + ctx + "</h2>\n"
                "  </div>\n"
                ");\n\n"
                "export default " + sub + ctx + ";\n");

            writeFile(subDir / "index.module.scss",
                ".page {\n  padding: 1.5rem;\n}\n");
        }
    }

    // ── Project config files ──────────────────────────────────────────────────

    writeFile(r / "package.json", R"({
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

    writeFile(r / "vite.config.ts", R"(import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react-swc'
import { resolve } from 'path'

export default defineConfig({
  plugins: [react()],
  resolve: { alias: { '@': resolve(__dirname, 'src') } }
})
)");

    writeFile(r / "tsconfig.json", R"({
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

    writeFile(r / "index.html", R"(<!DOCTYPE html>
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

    writeFile(r / ".gitignore", "node_modules/\ndist/\n.env\n.env.local\n*.local\n");

    writeFile(src / "app" / "main.tsx", R"(import React from 'react';
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

    writeFile(src / "app" / "App.tsx", R"(import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';

const App: React.FC = () => (
  <BrowserRouter>
    <Routes>
      <Route path="/" element={<div>Zeus Retail Evolution</div>} />
    </Routes>
  </BrowserRouter>
);
export default App;
)");

    writeFile(src / "store" / "index.ts", R"(import { configureStore } from '@reduxjs/toolkit';

export const store = configureStore({ reducer: {} });

export type RootState   = ReturnType<typeof store.getState>;
export type AppDispatch = typeof store.dispatch;
)");

    writeFile(src / "styles" / "global.scss",
        "* { box-sizing: border-box; margin: 0; padding: 0; }\n"
        "body { font-family: var(--font-family-primary, 'Figtree', sans-serif); }\n");

    writeFile(src / "components" / "common" / "HeaderGlobal.tsx", R"(import React from 'react';
import { Button } from '@giro-ds/react';

export const HeaderGlobal: React.FC = () => (
  <header className="header-global">
    <h2>Zeus Retail Evolution</h2>
    <Button variant="filled">PDV Online</Button>
  </header>
);
)");
}

} // namespace Infra
