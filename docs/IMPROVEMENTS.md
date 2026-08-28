Este gerador escrito em C++20 cria toda a estrutura de diretórios e os arquivos de boilerplate para o frontend em **Electron + React + `@giro-ds**` (respeitando as regras de *co-location* e Design System) e para o backend em **Laravel DDD** (com os 14 contextos mapeados do sistema Zeus Retail Evolution).

### Código C++ (`scaffold_zeus.cpp`)

```cpp
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void createFile(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream file(path);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void generateFrontendScaffold(const fs::path& baseDir) {
    fs::path src = baseDir / "frontend" / "src";

    // 1. Estrutura de Diretórios do Frontend (baseado na arquitetura giro-ds)
    std::vector<fs::path> directories = {
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
        src / "utils"
    };

    for (const auto& dir : directories) {
        fs::create_directories(dir);
    }

    // 2. Arquivos do Frontend
    createFile(baseDir / "frontend" / "package.json", R"({
  "name": "zeus-retail-frontend",
  "version": "1.0.0",
  "main": "electron/main.js",
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "electron:start": "electron ."
  },
  "dependencies": {
    "@giro-ds/react": "^1.0.0",
    "@reduxjs/toolkit": "^2.0.0",
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "react-redux": "^9.0.0",
    "react-router-dom": "^6.20.0"
  },
  "devDependencies": {
    "@types/react": "^18.2.0",
    "@vitejs/plugin-react": "^4.2.0",
    "electron": "^28.0.0",
    "sass": "^1.69.0",
    "typescript": "^5.2.0",
    "vite": "^5.0.0"
  }
})");

    createFile(src / "app" / "main.tsx", R"(import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider } from 'react-redux';
import { store } from '../store';
import App from './App';
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

export const App: React.FC = () => {
  return <AppRoutes />;
};

export default App;
)");

    createFile(src / "components" / "common" / "HeaderGlobal.tsx", R"(import React from 'react';
// Prioridade 1: Utilizando componentes do @giro-ds
import { Button, Navbar } from '@giro-ds/react';

export const HeaderGlobal: React.FC = () => {
  return (
    <Navbar className="header-global">
      <h2>Zeus Retail Evolution</h2>
      <Button variant="primary">PDV Online</Button>
    </Navbar>
  );
};
)");

    createFile(src / "pages" / "Dashboard" / "index.tsx", R"(import React from 'react';
import { DashboardCard } from './components/DashboardCard';

export const DashboardPage: React.FC = () => {
  return (
    <div className="dashboard-page">
      <h1>Dashboard - Zeus Retail</h1>
      <DashboardCard title="Vendas do Dia" value="R$ 12.450,00" />
    </div>
  );
};

export default DashboardPage;
)");

    createFile(src / "pages" / "Dashboard" / "components" / "DashboardCard.tsx", R"(import React from 'react';
import { Card } from '@giro-ds/react';

interface Props {
  title: string;
  value: string;
}

// Regra de Co-location: Componentes exclusivos de uma página ficam em pages/X/components/
export const DashboardCard: React.FC<Props> = ({ title, value }) => {
  return (
    <Card>
      <h3>{title}</h3>
      <p>{value}</p>
    </Card>
  );
};
)");
}

void generateBackendScaffold(const fs::path& baseDir) {
    fs::path appDir = baseDir / "backend" / "app" / "Contexts";

    // 14 Contextos DDD do Zeus Retail Evolution
    std::vector<std::string> contexts = {
        "Autenticacao", "Clientes", "Compartilhado", "Devolucoes", "Lojas",
        "Mercadorias", "Promocoes", "Vendas", "Tesouraria", "Finalizadoras",
        "Funcionarios", "Pdvs", "Combos", "Documentacao"
    };

    for (const auto& ctx : contexts) {
        fs::path ctxPath = appDir / ctx;

        // Camadas por Contexto DDD
        fs::create_directories(ctxPath / "Application" / "DTOs" / "Inputs");
        fs::create_directories(ctxPath / "Application" / "DTOs" / "Outputs");
        fs::create_directories(ctxPath / "Application" / "Errors");
        fs::create_directories(ctxPath / "Application" / "Exceptions");
        fs::create_directories(ctxPath / "Application" / "Queries");
        fs::create_directories(ctxPath / "Application" / "UseCases");
        fs::create_directories(ctxPath / "Application" / "Services");

        fs::create_directories(ctxPath / "Domain" / "Entities");
        fs::create_directories(ctxPath / "Domain" / "Enums");
        fs::create_directories(ctxPath / "Domain" / "Filters");
        fs::create_directories(ctxPath / "Domain" / "Autorizacoes");

        fs::create_directories(ctxPath / "Infra" / "Persistence" / "Models");
        fs::create_directories(ctxPath / "Infra" / "Persistence" / "Repositories");
        fs::create_directories(ctxPath / "Infra" / "Presentation" / "Http" / "Controllers");
        fs::create_directories(ctxPath / "Infra" / "Presentation" / "Routes");
        fs::create_directories(ctxPath / "Infra" / "Providers");

        // ServiceProvider Padrão do Contexto
        std::string providerContent = "<?php\n\ndeclare(strict_types=1);\n\nnamespace App\\Contexts\\" + ctx + "\\Infra\\Providers;\n\n"
            "use App\\Contexts\\Compartilhado\\Base\\Infra\\Providers\\CompartilhadoServiceProvider;\n"
            "use Illuminate\\Foundation\\Application;\n\n"
            "class " + ctx + "ServiceProvider extends CompartilhadoServiceProvider\n{\n"
            "    public function __construct(Application $app)\n    {\n"
            "        parent::__construct($app);\n"
            "        $this->setPrefix('" + ctx + "');\n"
            "        $this->setName('" + ctx + "');\n"
            "        $this->setRoute(__DIR__ . '/../Presentation/Routes/api.php');\n"
            "    }\n}\n";

        createFile(ctxPath / "Infra" / "Providers" / (ctx + "ServiceProvider.php"), providerContent);

        // Rotas Padrão
        std::string routeContent = "<?php\n\ndeclare(strict_types=1);\n\nuse Illuminate\\Support\\Facades\\Route;\n\n"
            "Route::get('/consultar', fn() => response()->json(['status' => 'OK', 'context' => '" + ctx + "']));\n";

        createFile(ctxPath / "Infra" / "Presentation" / "Routes" / "api.php", routeContent);
    }
}

int main() {
    fs::path projectRoot = fs::current_path() / "ZeusRetailEvolution";

    std::cout << "Gerando estrutura do Zeus Retail Evolution em: " << projectRoot << std::endl;

    try {
        generateFrontendScaffold(projectRoot);
        generateBackendScaffold(projectRoot);
        std::cout << "Scaffold gerado com sucesso!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erro ao gerar estrutura: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

```

---

### Instruções para Compilação e Execução

```bash
# Compilar com suporte ao C++17 / C++20
g++ -std=c++20 scaffold_zeus.cpp -o scaffold_zeus

# Executar o gerador no diretório desejado
./scaffold_zeus

```

---

### Mapeamento da Estrutura Gerada

```
ZeusRetailEvolution/
├── frontend/
│   ├── package.json                   # Dependências do GiroDS, React e Electron
│   └── src/
│       ├── app/                       # Entrypoints (App.tsx, main.tsx)
│       ├── components/                # Componentes globais (reaproveitados em 2+ páginas)
│       │   ├── common/                # Usa lib proprietária @giro-ds
│       │   └── layout/
│       ├── hooks/                     # Custom hooks globais
│       ├── layouts/                   # Templates de página
│       ├── pages/                     # Páginas da aplicação
│       │   ├── Dashboard/
│       │   │   ├── components/        # Co-location: componentes locais exclusivos
│       │   │   └── index.tsx
│       │   └── Tesouraria/
│       ├── routes/                    # Definição e renderização de rotas
│       ├── store/                     # Redux toolkit
│       ├── styles/                    # Sass/SCSS globais
│       ├── types/                     # Interfaces TypeScript
│       └── utils/                     # Métodos auxiliares puros
│
└── backend/
    └── app/
        └── Contexts/                  # 14 Contextos DDD
            ├── Autenticacao/
            ├── Clientes/
            ├── Devolucoes/
            ├── Tesouraria/
            │   ├── Application/
            │   │   ├── DTOs/
            │   │   ├── Errors/
            │   │   ├── Exceptions/
            │   │   ├── Queries/
            │   │   ├── Services/
            │   │   └── UseCases/
            │   ├── Domain/
            │   │   ├── Autorizacoes/
            │   │   ├── Entities/
            │   │   ├── Enums/
            │   │   └── Filters/
            │   └── Infra/
            │       ├── Persistence/
            │       │   ├── Models/
            │       │   └── Repositories/
            │       ├── Presentation/
            │       │   ├── Http/Controllers/
            │       │   └── Routes/api.php
            │       └── Providers/
            │           └── TesourariaServiceProvider.php
            └── ... (Demais contextos)

```

---
applyTo: 'back/app/Contexts/**'
---

# Arquitetura de Contexts — Zeus Retail Evolution

> Regras obrigatórias para geração de código em `back/app/Contexts/`.
> Seguem DDD + Clean Architecture. Desvios devem ser aprovados explicitamente.

---

## Gerar um novo Context

```bash
php artisan make:context NomeContext
# opções: --prefix=rota-custom  --sem-entity  --sem-autorizacoes
```

O comando cria toda a estrutura de diretórios, ServiceProvider, rotas, controller, exception, error, entity, repository, query e usecase skeleton, e registra o provider em `bootstrap/providers.php`.

---

## Estrutura obrigatória de diretórios

```
Contexts/[Nome]/
├── Application/
│   ├── DTOs/Inputs/          # readonly class — parâmetros tipados
│   ├── DTOs/Outputs/         # class (não readonly quando usa PaginacaoOutput)
│   ├── Errors/               # extends BaseError — erros para o usuário
│   ├── Exceptions/           # extends BaseException — lançada pelo UseCase/Query
│   ├── Queries/              # apenas GET — nunca escrevem dados
│   ├── Services/             # lógica complexa de negócio
│   └── UseCases/             # apenas POST/PUT/DELETE
├── Domain/
│   ├── Entities/             # inicializar sempre com ::create() / ::update()
│   ├── Enums/
│   ├── Filters/
│   └── Autorizacoes/         # regras de autorização (quando aplicável)
└── Infra/
    ├── Persistence/Models/
    ├── Persistence/Repositories/
    ├── Presentation/Http/Controllers/
    ├── Presentation/Http/Requests/
    ├── Presentation/Routes/api.php
    └── Providers/[Nome]ServiceProvider.php
```

---

## ServiceProvider

```php
// Infra/Providers/[Nome]ServiceProvider.php
class [Nome]ServiceProvider extends CompartilhadoServiceProvider
{
    public function __construct(Application $app)
    {
        parent::__construct($app);
        $this->setPrefix('nome-em-kebab-case');
        $this->setName('nome-em-kebab-case');
        $this->setRoute(__DIR__ . '/../Presentation/Routes/api.php');
    }
}
```

Registrar em `bootstrap/providers.php` (o `make:context` faz isso automaticamente).

---

## Rotas — padrões obrigatórios

| HTTP   | Caminho             | Método do controller |
|--------|---------------------|----------------------|
| GET    | `/consultar`        | `consultar`          |
| GET    | `/detalhar/{id}`    | `detalhar`           |
| POST   | `/criar`            | `criar`              |
| PUT    | `/alterar/{id}`     | `alterar`            |
| DELETE | `/deletar/{id}`     | `deletar` (opcional) |

- Sempre minúsculo com hífen (`-`), nunca camelCase
- Sub-recursos: `/api/[context]/[sub-recurso]/[método]`

```php
// Infra/Presentation/Routes/api.php
Route::get('/consultar', [NomeController::class, 'consultar'])->name('consultar');
Route::get('/detalhar/{id}', [NomeController::class, 'detalhar'])->name('detalhar');
Route::post('/criar', [NomeController::class, 'criar'])->name('criar');
Route::put('/alterar/{id}', [NomeController::class, 'alterar'])->name('alterar');
```

---

## Fluxo de dados por tipo de operação

**GET → Query**
```
Controller → Query → Repository → Database
```

**POST / PUT / DELETE → UseCase**
```
Controller → Request → UseCase → (Service?) → Entity → Repository → Database
```

---

## DTOs de Input — `readonly class`

```php
declare(strict_types=1);

readonly class CriarClienteInput
{
    public function __construct(
        public string $nome,
        public EmailVO $email,
        public IdVO $codLoja,
    ) {}
}
```

---

## DTOs de Output — dois tipos

**Simples (readonly):**
```php
readonly class DetalharClienteOutput
{
    public function __construct(
        public IdVO $id,
        public string $nome,
    ) {}

    public function toArray(): array
    {
        return ['id' => $this->id->getValue(), 'nome' => $this->nome];
    }
}
```

**Listagem paginada (extends PaginacaoOutput):**
```php
class ConsultarClientesOutput extends PaginacaoOutput
{
    public function adicionarRegistro(   // nome livre: adicionarItem, adicionarCliente, etc.
        IdVO $id,
        string $nome,
        Carbon $dataCadastro,
    ): void {
        $this->data[] = [
            'id'            => $id->getValue(),
            'nome'          => $nome,
            'data_cadastro' => $dataCadastro->format('Y-m-d'),
        ];
    }
}
```

---

## Query (GET)

```php
class ConsultarClientesQuery
{
    public function __construct(
        private ClienteRepository $clienteRepository,
    ) {}

    public function executar(PaginacaoInput $input): ConsultarClientesOutput
    {
        $output    = new ConsultarClientesOutput();
        $registros = $this->clienteRepository->obterListaPorPaginacao($input);

        foreach ($registros as $registro) {
            $output->adicionarRegistro(
                id:           new IdVO((int) $registro->cod_cliente),
                nome:         $registro->des_cliente,
                dataCadastro: Carbon::parse($registro->data_cadastro),
            );
        }

        $output->page     = $registros->currentPage();
        $output->lastPage = $registros->lastPage();
        $output->total    = $registros->total();

        return $output;
    }
}
```

---

## UseCase (POST/PUT/DELETE)

```php
class CriarClienteUseCase
{
    public function __construct(
        private ClienteRepository $clienteRepository,
    ) {}

    public function executar(CriarClienteInput $input): IdVO
    {
        $existente = $this->clienteRepository->obterPorEmail($input->email);
        if ($existente) {
            throw new ClienteException(new ClienteJaExisteError());
        }

        $entity = ClienteEntity::create(nome: $input->nome, email: $input->email);
        return $this->clienteRepository->criar($entity);
    }
}
```

---

## Entity

```php
class ClienteEntity
{
    private function __construct(
        public readonly ?IdVO $id,
        public readonly string $nome,
        public readonly EmailVO $email,
    ) {}

    public static function create(string $nome, EmailVO $email): self
    {
        // validações aqui
        return new self(id: null, nome: $nome, email: $email);
    }

    public static function update(IdVO $id, string $nome): self
    {
        return new self(id: $id, nome: $nome, email: new EmailVO(''));
    }
}
```

**Nunca instanciar Entity com `new` diretamente.** Sempre usar `::create()` ou `::update()`.

---

## Repository

```php
class ClienteRepository
{
    // ✅ Retorna OBJETO Eloquent, nunca ->toArray()
    public function obterPorId(IdVO $id): ?object
    {
        return Cliente::find($id->getValue());
    }

    public function obterListaPorPaginacao(PaginacaoInput $input): LengthAwarePaginator
    {
        return Cliente::select(['cod_cliente', 'des_cliente', 'data_cadastro'])
            ->paginate(perPage: $input->perPage, page: $input->page);
    }

    public function criar(ClienteEntity $entity): IdVO
    {
        $model = Cliente::create(['des_cliente' => $entity->nome]);
        return new IdVO($model->cod_cliente);
    }
}
```

---

## Errors e Exceptions

**Exception do context (um por context):**
```php
// Application/Exceptions/ClienteException.php
class ClienteException extends BaseException {}
```

**Error específico:**
```php
// Application/Errors/ClienteNaoEncontradoError.php
class ClienteNaoEncontradoError extends BaseError
{
    protected int $code = 404;
    protected string $message = 'Cliente não encontrado';
}
```

**Códigos HTTP:** 400 Bad Request · 401 Unauthorized · 403 Forbidden · 404 Not Found · 409 Conflict · 422 Unprocessable · 500 Internal Error

**Uso no UseCase:**
```php
throw new ClienteException(new ClienteNaoEncontradoError());
```

---

## Controller

```php
class ClienteController extends Controller
{
    public function consultar(Request $request, ConsultarClientesQuery $query): JsonResponse
    {
        $input  = new PaginacaoInput(page: $request->page(1), perPage: $request->perPage(20));
        $output = $query->executar($input);
        return $this->successResponse(data: $output->toArray());
    }

    public function criar(CriarClienteRequest $request, CriarClienteUseCase $useCase): JsonResponse
    {
        $input = new CriarClienteInput(nome: $request->nome, email: new EmailVO($request->email));
        $id    = $useCase->executar($input);
        return $this->successResponse(data: ['id' => $id->getValue()], status: 201);
    }
}
```

---

## Value Objects disponíveis

```php
// Base
use App\Contexts\Compartilhado\Base\Domain\VOs\IdVO;        // IDs numéricos
use App\Contexts\Compartilhado\Base\Domain\VOs\EmailVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\PhoneVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\CepVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\MoneyVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\DecimalVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\DocumentVO;  // CPF/CNPJ automático

// Documentos específicos
use App\Contexts\Compartilhado\Base\Domain\VOs\Documents\CPFVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\Documents\CNPJVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\Documents\RGVO;
use App\Contexts\Compartilhado\Base\Domain\VOs\Documents\RNEVO;

// Enums compartilhados
use App\Contexts\Compartilhado\Base\Domain\Enums\TipoPessoaEnum;
use App\Contexts\Compartilhado\Base\Domain\Enums\UFEnum;
```

Procurar VOs existentes antes de criar novos.

---

## Regras absolutas — Arrays Associativos

| Local                          | Array associativo permitido? |
|-------------------------------|------------------------------|
| DTO Output `toArray()`         | ✅ SIM                        |
| DTO Output métodos `adicionar*`| ✅ SIM                        |
| Repository                     | ❌ NÃO — retornar objeto       |
| Query / UseCase                | ❌ NÃO                        |
| Entity / Service               | ❌ NÃO                        |

```php
// ❌ PROIBIDO em Repository
return $model->toArray();

// ❌ PROIBIDO em Query/UseCase
$dados = ['campo' => $valor];
$item  = (object) ['cod' => $cod];

// ❌ PROIBIDO — acessar Eloquent como array
$nome = $model['des_cliente'];

// ✅ CORRETO — Repository retorna objeto
return $model;

// ✅ CORRETO — Query usa atributos
$registro->des_cliente;

// ✅ CORRETO — passa diretamente para o método tipado
$output->adicionarRegistro(id: new IdVO((int) $registro->cod_cliente), nome: $registro->des_cliente);
```

---

## Regras de nomenclatura

- `declare(strict_types=1)` em TODOS os arquivos PHP
- **UseCases** → apenas POST/PUT/DELETE
- **Queries** → apenas GET
- **Repository** → único com SQL direto
- **Entity** → nunca `new Entity()`, sempre `::create()` / `::update()`
- Rotas sempre em **minúsculo com hífen**, nunca camelCase

---

## Checklist antes de finalizar código

- [ ] Repository retorna objeto Eloquent (`return $model`)
- [ ] Nenhum `->toArray()` no Repository
- [ ] Nenhum array associativo fora de DTO Output
- [ ] Nenhum `(object) [...]` fora de DTO Output
- [ ] DTOs Input são `readonly class`
- [ ] Entity inicializada via método estático
- [ ] Erros de usuário usam Exception + Error
- [ ] Rotas em minúsculo com hífen
- [ ] `declare(strict_types=1)` presente
- [ ] UseCase ≠ GET, Query ≠ POST/PUT/DELETE
