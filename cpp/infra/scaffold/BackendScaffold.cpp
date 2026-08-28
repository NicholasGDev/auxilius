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

void generateBackendScaffold(const fs::path& root)
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

        const std::string ctxLower = toLower(ctx);

        writeFile(c / "Infra" / "Providers" / (ctx + "ServiceProvider.php"),
            "<?php\n\ndeclare(strict_types=1);\n\n"
            "namespace App\\Contexts\\" + ctx + "\\Infra\\Providers;\n\n"
            "use App\\Contexts\\Compartilhado\\Base\\Infra\\Providers\\CompartilhadoServiceProvider;\n"
            "use Illuminate\\Contracts\\Foundation\\Application;\n\n"
            "class " + ctx + "ServiceProvider extends CompartilhadoServiceProvider\n{\n"
            "    public function __construct(Application $app)\n    {\n"
            "        parent::__construct($app);\n"
            "        $this->setPrefix('" + ctxLower + "');\n"
            "        $this->setName('" + ctxLower + "');\n"
            "        $this->setRoute(__DIR__ . '/../Presentation/Routes/api.php');\n"
            "    }\n\n"
            "    public function boot(): void\n    {\n"
            "        parent::boot();\n"
            "    }\n}\n");

        writeFile(c / "Infra" / "Presentation" / "Routes" / "api.php",
            "<?php\n\ndeclare(strict_types=1);\n\n"
            "use App\\Contexts\\" + ctx + "\\Infra\\Presentation\\Http\\Controllers\\" + ctx + "Controller;\n"
            "use Illuminate\\Support\\Facades\\Route;\n\n"
            "Route::middleware('permissao.contexto:" + ctxLower + "')->group(function () {\n"
            "    Route::get('/consultar', [" + ctx + "Controller::class, 'consultar'])->name('consultar');\n"
            "    Route::get('/detalhar/{id}', [" + ctx + "Controller::class, 'detalhar'])->name('detalhar');\n"
            "    Route::post('/criar', [" + ctx + "Controller::class, 'criar'])->name('criar');\n"
            "    Route::put('/alterar/{id}', [" + ctx + "Controller::class, 'alterar'])->name('alterar');\n"
            "    Route::delete('/deletar/{id}', [" + ctx + "Controller::class, 'deletar'])->name('deletar');\n"
            "});\n");
    }

    std::string providers = "<?php\n\nreturn [\n";
    for (const auto& ctx : contexts)
        providers += "    App\\Contexts\\" + ctx + "\\Infra\\Providers\\" + ctx + "ServiceProvider::class,\n";
    providers += "];\n";
    writeFile(root / "back" / "bootstrap" / "providers.php", providers);

    writeFile(root / "back" / ".gitignore", R"(/vendor/
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

} // namespace Infra
