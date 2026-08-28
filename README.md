# Auxilius — Zeus Retail Evolution Developer Toolbox

> Desktop app (Electron + React + Giro DS) powered by a C++ CLI engine using DDD architecture.

---

## Project Structure

```
auxilius/
├── bin/                        # Compiled C++ binary (scaffold_zeus) — gitignored
├── cpp/                        # C++20 source — DDD architecture
│   ├── domain/
│   │   └── Field.hpp           # Domain entities: Field, EndpointConfig, GeneratedFile
│   ├── shared/
│   │   ├── Json.hpp            # JSON serialization helpers
│   │   └── FileSystem.hpp      # writeFile, mkdirs, fileExists
│   ├── application/
│   │   ├── ScaffoldUseCase.*   # Scaffold generation orchestration
│   │   ├── EndpointUseCase.*   # Endpoint preview/generate (outputs JSON)
│   │   └── EnvironmentUseCase.*# Environment check/setup orchestration
│   ├── infra/
│   │   ├── scaffold/
│   │   │   ├── Scaffold.hpp
│   │   │   ├── BackendScaffold.cpp   # Laravel DDD: 14 contexts + routes
│   │   │   ├── FrontendScaffold.cpp  # React + Giro DS scaffold
│   │   │   └── ElectronScaffold.cpp  # Electron-vite scaffold
│   │   ├── templates/
│   │   │   ├── PhpTemplates.hpp      # All PHP DDD artifact templates
│   │   │   └── PhpTemplates.cpp      # InputDTO, UseCase, Query, Entity, Repository…
│   │   └── environment/
│   │       ├── Environment.hpp       # Environment checks + setup declarations
│   │       └── Environment.cpp       # git, nvm, node20, WSL, project checks
│   ├── main.cpp                # CLI entry point
│   └── CMakeLists.txt          # CMake build config
├── src/
│   ├── main/                   # Electron main process (Node16 TypeScript)
│   │   ├── index.ts
│   │   └── ipc/
│   │       ├── scaffold.ts     # IPC → C++ scaffold commands
│   │       ├── endpoints.ts    # IPC → C++ endpoint preview/generate
│   │       └── environment.ts  # IPC → C++ environment commands
│   ├── preload/
│   │   └── index.ts            # contextBridge: exposes window.auxilius
│   └── renderer/src/           # React app (ESNext + bundler resolution)
│       ├── pages/
│       │   ├── Welcome/        # Welcome page: new/existing project selection
│       │   ├── Home/           # Dashboard: environment status
│       │   ├── Setup/          # Environment setup actions
│       │   ├── Scaffold/       # Run scaffold generator
│       │   └── Endpoints/      # Endpoint generator + VSCode-like file preview
│       ├── components/
│       │   ├── layout/MainLayout.tsx
│       │   └── common/Sidebar.tsx
│       ├── contexts/
│       │   └── ProjectContext.tsx  # Active project path context
│       ├── routes/index.tsx
│       ├── styles/global.scss
│       └── App.tsx
├── scripts/
│   └── docker-build.sh         # Builds C++ via Docker
├── docs/
│   └── IMPROVEMENTS.md         # Historical improvement notes
├── Dockerfile                  # Multi-stage: g++12 → node20-slim → artifacts
├── docker-compose.yml
├── electron.vite.config.ts
├── tsconfig.json               # Project references (files: [])
├── tsconfig.node.json          # main + preload: Node16 resolution
├── tsconfig.web.json           # renderer: bundler + ESNext resolution
└── package.json
```

---

## C++ CLI

The `bin/scaffold_zeus` binary is the core engine. All generation logic lives in C++.

```bash
# Scaffold a new Zeus project
scaffold_zeus scaffold /path/to/target

# Preview endpoint files (outputs JSON)
scaffold_zeus endpoint --context=Vendas --resource=Pedido \
  --operation=criar --fields=cliente:string,valor:decimal \
  --project=/path/to/zeus --preview

# Generate endpoint files to disk
scaffold_zeus endpoint ... --generate

# Environment checks/setup
scaffold_zeus env check
scaffold_zeus env setup-wsl
scaffold_zeus env setup-nvm
scaffold_zeus env clone-project
scaffold_zeus env git-config
```

**Fields format:** `name:type[?]` — append `?` for nullable.  
Types: `string`, `decimal`, `int`, `bool`, `uuid`, `date`, `datetime`, or any VO name.

---

## Build

### Option 1 — Docker (recommended)

```bash
npm run cpp:build    # Builds C++ binary via docker-compose cpp-build service
```

### Option 2 — Direct g++ (requires g++ 12+ with C++20)

```bash
npm run cpp:compile
```

### Run the Electron app

```bash
npm install
npm run dev
```

---

## Tech Stack

| Layer        | Technology                                  |
|--------------|---------------------------------------------|
| Desktop      | Electron 28, electron-vite 2                |
| UI           | React 18, TypeScript 5, Giro DS 9           |
| Styling      | SCSS, Giro DS tokens                        |
| Engine       | C++20 (DDD), compiled to native binary      |
| Build        | Docker (gcc:12), node:20-slim               |
| IPC          | contextBridge + ipcMain.handle              |

---

## Pages

| Page            | Route        | Description                                      |
|-----------------|--------------|--------------------------------------------------|
| Welcome         | (root)       | Select new or existing Zeus project (WSL path)   |
| Dashboard       | `/home`      | Environment status: git, nvm, node, WSL, project |
| Ambiente        | `/setup`     | Run environment setup commands                   |
| Scaffold        | `/scaffold`  | Generate complete Zeus project structure         |
| Endpoints       | `/endpoints` | Generate PHP DDD artifacts (VSCode-like preview) |
