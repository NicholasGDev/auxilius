#include "Scaffold.hpp"
#include "../../shared/FileSystem.hpp"
#include <string>

namespace Infra {

using Shared::writeFile;
using Shared::mkdirs;
namespace fs = std::filesystem;

void generateFrontendScaffold(const fs::path& root)
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
import { AppRoutes } from '../routes';

const App: React.FC = () => <AppRoutes />;
export default App;
)");

    writeFile(src / "routes" / "index.tsx", R"(import React from 'react';
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

    writeFile(src / "pages" / "Dashboard" / "index.tsx", R"(import React from 'react';
import { DashboardCard } from './components/DashboardCard';

export const DashboardPage: React.FC = () => (
  <div className="dashboard-page">
    <h1>Dashboard — Zeus Retail</h1>
    <DashboardCard title="Vendas do Dia" value="R$ 12.450,00" />
  </div>
);
)");

    writeFile(src / "pages" / "Dashboard" / "components" / "DashboardCard.tsx", R"(import React from 'react';

interface Props { title: string; value: string; }

export const DashboardCard: React.FC<Props> = ({ title, value }) => (
  <div className="dashboard-card">
    <h3>{title}</h3>
    <p>{value}</p>
  </div>
);
)");
}

} // namespace Infra
