# Auxilius — Caixa de Ferramentas Zeus Retail Evolution

> App desktop (Electron + React + Giro DS) com motor C++20 em arquitetura DDD.  
> Layout inspirado no VS Code — Activity Bar, Explorer, File Tree, Tab Bar, Status Bar.  
> Persiste configurações em SQLite via binário C++. Sem Docker. Suporte a WSL.

---

## 📥 Instalação / Download

> Todas as versões disponíveis em: **[Releases do GitHub](https://github.com/zanthustecnologia/auxilius/releases/latest)**

### 🐧 Linux — Pacote `.deb` (Ubuntu / Debian) ✅ Recomendado

Instalável e desinstalável pelo sistema (apt / dpkg):

```bash
# 1. Baixar o pacote
wget https://github.com/zanthustecnologia/auxilius/releases/latest/download/auxilius_0.1.0_amd64.deb

# 2. Instalar
sudo dpkg -i auxilius_0.1.0_amd64.deb

# Caso falte alguma dependência, execute:
sudo apt-get install -f

# 3. Iniciar o app
auxilius
```

**Desinstalar:**

```bash
# Remove o app (mantém configurações)
sudo dpkg -r auxilius
# ou via apt:
sudo apt remove auxilius

# Remove tudo incluindo /opt/auxilius (purge completo)
sudo dpkg -P auxilius
```

O `.deb` registra o app no sistema, aparece no **Software Center** e instala automaticamente `nvm` + Node.js 20 no `postinst`.

---

### 🐧 Linux — Instalador Gráfico GTK3 (Wizard)

Para distribuições que não usam `.deb` (Fedora, Arch, etc.) ou preferência por assistente visual:

```bash
# 1. Baixar e extrair
wget https://github.com/zanthustecnologia/auxilius/releases/latest/download/auxilius-installer-linux-x64.tar.gz
tar -xzf auxilius-installer-linux-x64.tar.gz

# 2. Executar o assistente (requer GTK3)
chmod +x auxilius-installer
./auxilius-installer
```

O wizard gráfico guia por 4 etapas: boas-vindas → escolha do diretório → instalação com log em tempo real → conclusão.

---

### 🪟 Windows — Instalador `.exe` (NSIS)

Registra no **Painel de Controle → Programas e Recursos** para desinstalação nativa.

```
1. Baixar:
   https://github.com/zanthustecnologia/auxilius/releases/latest/download/auxilius-setup-0.1.0.exe

2. Executar auxilius-setup-0.1.0.exe como administrador (recomendado)

3. Seguir o assistente de instalação

4. O app é instalado em %LOCALAPPDATA%\Auxilius
   com atalhos no Menu Iniciar e Área de Trabalho
```

**Desinstalar no Windows:**
- Painel de Controle → Programas → Programas e Recursos → **Auxilius** → Desinstalar
- Ou: Configurações → Aplicativos → **Auxilius** → Desinstalar

---

### 🪟 Windows com Projeto no WSL

O Electron roda no Windows, mas o projeto `zeus-retail-evolution` pode ficar dentro do WSL2:

```
1. No seletor "Escolher Projeto", navegue para o caminho WSL:
   \\wsl$\Ubuntu\root\projects\zeus-retail-evolution

2. O app detecta automaticamente o caminho WSL e roteia
   todos os comandos C++ via wsl.exe -d Ubuntu

3. Requisitos:
   - WSL2 instalado com Ubuntu
   - scaffold_zeus compilado dentro do WSL em ~/auxilius/bin/
```

Os binários C++ (`scaffold_zeus`) vivem no WSL. O Electron traduz os caminhos automaticamente via `src/main/wsl.ts`.

---

### 📦 Instalador Universal (Shell — Linux/macOS)

```bash
bash scripts/install.sh
```

Detecta o SO e executa automaticamente:
1. Instala `g++`, `libsqlite3-dev`, `git`, `curl` (apt / brew)
2. Instala `nvm` + Node.js 20 LTS
3. Executa `npm install`
4. Compila o binário C++ com `-lsqlite3`
5. Cria atalho `.desktop` (Linux)

---

## Estrutura do Projeto

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
│   │   ├── database/
│   │   │   ├── Database.hpp    # SQLite wrapper (~/.auxilius/auxilius.db)
│   │   │   └── Database.cpp    # set / get / list — key-value store
│   │   ├── scaffold/
│   │   │   ├── Scaffold.hpp
│   │   │   ├── BackendScaffold.cpp   # Laravel DDD: 14 contexts + middleware routes
│   │   │   ├── FrontendScaffold.cpp  # React + Giro DS: per-context sub-pages
│   │   │   └── ElectronScaffold.cpp  # Electron-vite scaffold
│   │   ├── templates/
│   │   │   ├── PhpTemplates.hpp      # All PHP DDD artifact templates
│   │   │   └── PhpTemplates.cpp      # InputDTO, UseCase, Query, Entity, Repository…
│   │   └── environment/
│   │       ├── Environment.hpp
│   │       └── Environment.cpp       # git, nvm, node20, WSL, project checks
│   ├── main.cpp                # CLI entry point
│   └── CMakeLists.txt
├── src/
│   ├── main/                   # Electron main process
│   │   ├── index.ts
│   │   └── ipc/
│   │       ├── scaffold.ts
│   │       ├── endpoints.ts
│   │       ├── environment.ts
│   │       └── database.ts     # IPC ↔ C++ db get/set/list
│   ├── preload/
│   │   └── index.ts            # contextBridge: window.auxilius (+ db namespace)
│   └── renderer/src/
│       ├── pages/
│       │   ├── Welcome/        # Welcome: selecionar projeto (path salvo no SQLite)
│       │   ├── Home/           # Dashboard: status do ambiente
│       │   ├── Setup/          # Configurar ambiente
│       │   ├── Scaffold/       # Gerar estrutura Zeus DDD
│       │   └── Endpoints/      # Gerar artefatos PHP DDD (preview tipo VS Code)
│       ├── components/
│       │   └── layout/
│       │       ├── MainLayout.tsx    # Shell VS Code (Activity Bar + Sidebar + Editor + Status Bar)
│       │       ├── ActivityBar.tsx   # Barra lateral esquerda com ícones (48px)
│       │       ├── FileTree.tsx      # Árvore de arquivos lazy-load
│       │       ├── ExplorerPanel.tsx # Painel do explorador
│       │       └── StatusBar.tsx     # Barra inferior (branch, projeto, versão)
│       ├── contexts/
│       │   ├── ProjectContext.tsx    # Caminho do projeto ativo
│       │   └── SettingsContext.tsx   # Configurações persistidas no SQLite via C++
│       ├── styles/
│       │   ├── global.scss           # Tokens, utilitários, tema escuro
│       │   └── vscode-layout.scss    # Layout VS Code (shell, activity bar, sidebar, tabs)
│       ├── types/
│       │   └── electron.d.ts         # window.auxilius types (inclui db namespace)
│       └── App.tsx                   # Restaura projectPath do SQLite no startup
├── scripts/
│   ├── install.sh              # Instalador universal (Linux/macOS) — sem Docker
│   └── docker-build.sh         # Build C++ via Docker (opcional)
├── docs/
│   └── IMPROVEMENTS.md
├── Dockerfile                  # Opcional — Multi-stage: g++12 → node20-slim
├── docker-compose.yml          # Opcional
├── electron.vite.config.ts
├── tsconfig.json
├── tsconfig.node.json
├── tsconfig.web.json
└── package.json
```

---

## C++ CLI

O binário `bin/scaffold_zeus` é o motor central. Toda lógica de geração e persistência fica em C++.

```bash
# Scaffold completo do projeto Zeus
scaffold_zeus scaffold /path/to/target

# Preview de arquivos de endpoint (retorna JSON)
scaffold_zeus endpoint --context=Vendas --resource=Pedido \
  --operation=criar --fields=cliente:string,valor:decimal \
  --project=/path/to/zeus --preview

# Gerar arquivos no disco
scaffold_zeus endpoint ... --generate

# Verificações de ambiente
scaffold_zeus env check
scaffold_zeus env setup-wsl
scaffold_zeus env setup-nvm
scaffold_zeus env clone-project
scaffold_zeus env git-config

# Banco de dados SQLite (~/.auxilius/auxilius.db)
scaffold_zeus db get projectPath
scaffold_zeus db set projectPath /root/projects/zeus-retail-evolution
scaffold_zeus db list
```

**Formato de campos:** `nome:tipo[?]` — adicione `?` para nullable.  
Tipos: `string`, `decimal`, `int`, `bool`, `uuid`, `date`, `datetime` ou qualquer nome de VO.

---

## Banco de Dados SQLite (C++)

As configurações são persistidas automaticamente em `~/.auxilius/auxilius.db` pelo binário C++.  
Não é necessário nenhum módulo Node nativo — o Electron comunica com o banco via IPC → spawn do binário.

**Chaves persistidas:**
| Chave             | Descrição                          |
|-------------------|------------------------------------|
| `projectPath`     | Caminho do projeto ativo           |
| `lastContext`     | Último contexto usado nos endpoints |
| `lastResource`    | Último resource usado              |
| `lastOperation`   | Última operação (consultar/criar…) |
| `lastFields`      | Últimos campos digitados           |
| `gitName`         | Nome para git config               |
| `gitEmail`        | Email para git config              |

---

## Layout VS Code

```
┌─────────────────────────────────────────────────────────────┐
│  Activity   │   Sidebar Panel    │    Editor Area            │
│   Bar       │   (Explorer /      │  ┌──────────────────────┐│
│  (48px)     │    Nav / Config)   │  │ Tabs: Home Setup …   ││
│             │   (240px)          │  ├──────────────────────┤│
│  📁 Explorer│                    │  │                      ││
│  ◈ Scaffold │  EXPLORADOR        │  │   Conteúdo da página ││
│  ⊕ Endpoints│  ├── back/         │  │                      ││
│  ⚙ Ambiente │  │   └── app/      │  │                      ││
│             │  └── front/        │  │                      ││
│  ↕ Trocar   │      └── src/      │  └──────────────────────┘│
├─────────────┴────────────────────┴──────────────────────────┤
│ ⎇ main   📁 zeus-retail-evolution    SCAFFOLD   Auxilius v0.1│
└─────────────────────────────────────────────────────────────┘
```

---

## Scaffold Gerado (baseado no projeto real zeus-retail-evolution)

### Backend — Laravel DDD (`back/app/Contexts/[Nome]/`)
Estrutura baseada em `/root/projects/zeus-retail-evolution/back/app/Contexts/Tesouraria`:
```
[Contexto]/
├── Application/
│   ├── DTOs/Inputs/          # readonly class — parâmetros tipados
│   ├── DTOs/Outputs/         # simples (readonly) ou paginados (extends PaginacaoOutput)
│   ├── Errors/               # extends BaseError (code HTTP + message)
│   ├── Exceptions/           # [Contexto]Exception extends BaseException (1 por contexto)
│   ├── Queries/              # GET: Controller → Query → Repository
│   ├── Services/             # lógica complexa
│   └── UseCases/             # POST/PUT/DELETE: Controller → UseCase → Entity → Repository
├── Domain/
│   ├── Entities/             # ::create() / ::update() — nunca new direto
│   ├── Enums/
│   ├── Filters/
│   └── Autorizacoes/
└── Infra/
    ├── Persistence/Models/
    ├── Persistence/Repositories/
    ├── Presentation/Http/Controllers/
    ├── Presentation/Http/Requests/
    ├── Presentation/Routes/api.php    # Route::middleware('permissao.contexto:...')
    └── Providers/[Nome]ServiceProvider.php
```

### Frontend — React + Giro DS (`front/src/app/pages/[Contexto]/`)
Estrutura baseada em `/root/projects/zeus-retail-evolution/front/src/app/pages/Treasury`:
```
[Contexto]/
├── Consultar/
│   ├── index.tsx
│   └── index.module.scss
├── Criar/
│   ├── index.tsx
│   └── index.module.scss
├── Detalhar/
│   ├── index.tsx
│   └── index.module.scss
├── index.tsx              # Página raiz (dashboard do contexto)
├── index.module.scss
└── [contexto].routes.ts   # Rotas do contexto
```

---

## Desenvolvimento

```bash
npm run dev                   # Electron + Vite dev server
npm run cpp:compile           # Recompila binário C++ (inclui SQLite)
npm run installer:linux       # Build instalador GTK3 → bin/auxilius-installer
npm run installer:linux:deb   # Build pacote .deb   → dist/auxilius_*.deb
npm run installer:windows     # Build .exe NSIS     → dist/auxilius-setup-*.exe
npm run install:universal     # Instalador shell (sem GUI)
```

**Pré-requisitos para build:**

| Artefato | Requisito extra |
|---|---|
| Binário C++ | `g++ 12+`, `libsqlite3-dev` |
| Pacote `.deb` | `dpkg-deb` (padrão no Debian/Ubuntu) |
| Wizard GTK3 | `libwxgtk3.2-dev`, `wx-config` |
| Instalador Windows | `makensis` (NSIS) |

---

## Publicar Nova Versão (GitHub Actions)

O workflow `.github/workflows/release.yml` compila e publica os 3 artefatos automaticamente ao criar uma tag:

```bash
git tag v0.1.0
git push origin v0.1.0
```

O GitHub Actions:
1. Compila `scaffold_zeus` + instalador GTK3 + `.deb` no runner `ubuntu-22.04`
2. Compila `scaffold_zeus.exe` + `.exe` NSIS no runner `windows-latest`
3. Cria uma [GitHub Release](https://github.com/zanthustecnologia/auxilius/releases) com os 3 arquivos para download

---

## Tech Stack

| Camada       | Tecnologia                                       |
|--------------|--------------------------------------------------|
| Desktop      | Electron 28, electron-vite 2                     |
| UI           | React 18, TypeScript 5, Giro DS 9                |
| Styling      | SCSS, Giro DS tokens, tema VS Code               |
| Layout       | Activity Bar + File Tree + Tab Bar + Status Bar  |
| Engine       | C++20 (DDD), compilado para binário nativo       |
| Persistência | SQLite via C++ (`~/.auxilius/auxilius.db`)       |
| Build        | g++ 12 + libsqlite3 (direto), Docker (opcional)  |
| IPC          | contextBridge + ipcMain.handle → spawn C++       |

---

## Páginas

| Página          | Rota         | Descrição                                              |
|-----------------|--------------|--------------------------------------------------------|
| Welcome         | (raiz)       | Selecionar projeto (path restaurado do SQLite)         |
| Dashboard       | `/home`      | Status do ambiente: git, nvm, node, WSL, projeto       |
| Ambiente        | `/setup`     | Executar comandos de configuração do ambiente          |
| Scaffold        | `/scaffold`  | Gerar estrutura completa do projeto Zeus DDD           |
| Endpoints       | `/endpoints` | Gerar artefatos PHP DDD (preview tipo VS Code)         |

