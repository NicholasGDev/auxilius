#include "PhpTemplates.hpp"
#include "../../shared/FileSystem.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>

namespace Infra {

// ─── Helpers ─────────────────────────────────────────────────────────────────

std::string PhpTemplates::ucfirst(const std::string& s)
{
    if (s.empty()) return s;
    std::string r = s;
    r[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(r[0])));
    return r;
}

std::string PhpTemplates::lcfirst(const std::string& s)
{
    if (s.empty()) return s;
    std::string r = s;
    r[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(r[0])));
    return r;
}

static bool endsWithVO(const std::string& t)
{
    return t.size() >= 2 && t.substr(t.size() - 2) == "VO";
}

// ─── Templates ───────────────────────────────────────────────────────────────

std::string PhpTemplates::tplInputDTO(const EndpointConfig& cfg)
{
    const std::string action  = ucfirst(cfg.operation);
    const std::string res     = ucfirst(cfg.resource);
    const std::string ctxNs   = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::string params;
    for (const auto& f : cfg.fields) {
        params += "        public readonly ";
        params += f.nullable ? "?" : "";
        params += f.voType + " $" + f.name + ",\n";
    }

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\DTOs\\Inputs;\n\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "readonly class " + action + res + "Input\n{\n"
           "    public function __construct(\n" + params +
           "    ) {}\n}\n";
}

std::string PhpTemplates::tplOutputSimple(const EndpointConfig& cfg)
{
    const std::string action   = ucfirst(cfg.operation);
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::string params, toArray;
    for (const auto& f : cfg.fields) {
        params  += "        public readonly " + f.voType + " $" + f.name + ",\n";
        toArray += "            '" + f.name + "' => "
                 + (endsWithVO(f.voType) ? "$this->" + f.name + "->getValue()" : "$this->" + f.name)
                 + ",\n";
    }

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\DTOs\\Outputs;\n\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "readonly class " + action + res + "Output\n{\n"
           "    public function __construct(\n"
           "        public readonly IdVO $id,\n" + params +
           "    ) {}\n\n"
           "    public function toArray(): array\n    {\n        return [\n"
           "            'id' => $this->id->getValue(),\n" + toArray +
           "        ];\n    }\n}\n";
}

std::string PhpTemplates::tplOutputPaginado(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::string addParams, data;
    for (const auto& f : cfg.fields) {
        addParams += "        " + f.voType + " $" + f.name + ",\n";
        data      += "            '" + f.name + "' => "
                  + (endsWithVO(f.voType) ? "$" + f.name + "->getValue()" : "$" + f.name)
                  + ",\n";
    }

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\DTOs\\Outputs;\n\n"
           "use " + sharedNs + "\\Application\\DTOs\\Outputs\\PaginacaoOutput;\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "class Consultar" + res + "sOutput extends PaginacaoOutput\n{\n"
           "    public function adicionar" + res + "(\n" + addParams +
           "    ): void {\n        $this->data[] = [\n" + data +
           "        ];\n    }\n}\n";
}

std::string PhpTemplates::tplUseCase(const EndpointConfig& cfg)
{
    const std::string action   = ucfirst(cfg.operation);
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";
    const std::string repoVar  = lcfirst(res) + "Repository";

    if (cfg.operation == "criar") {
        std::string entityFields;
        for (const auto& f : cfg.fields)
            entityFields += "            " + f.name + ": $input->" + f.name + ",\n";

        return "<?php\n\ndeclare(strict_types=1);\n\n"
               "namespace " + ctxNs + "\\Application\\UseCases;\n\n"
               "use " + ctxNs + "\\Application\\DTOs\\Inputs\\" + action + res + "Input;\n"
               "use " + ctxNs + "\\Application\\Exceptions\\" + res + "Exception;\n"
               "use " + ctxNs + "\\Domain\\Entities\\" + res + "Entity;\n"
               "use " + ctxNs + "\\Infra\\Persistence\\Repositories\\" + res + "Repository;\n"
               "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
               "class " + action + res + "UseCase\n{\n"
               "    public function __construct(\n"
               "        private " + res + "Repository $" + repoVar + ",\n"
               "    ) {}\n\n"
               "    public function executar(" + action + res + "Input $input): IdVO\n    {\n"
               "        $entity = " + res + "Entity::create(\n" + entityFields +
               "        );\n\n        return $this->" + repoVar + "->criar($entity);\n"
               "    }\n}\n";
    }

    if (cfg.operation == "alterar") {
        std::string entityFields;
        for (const auto& f : cfg.fields)
            entityFields += "            " + f.name + ": $input->" + f.name + ",\n";

        return "<?php\n\ndeclare(strict_types=1);\n\n"
               "namespace " + ctxNs + "\\Application\\UseCases;\n\n"
               "use " + ctxNs + "\\Application\\DTOs\\Inputs\\" + action + res + "Input;\n"
               "use " + ctxNs + "\\Application\\Errors\\" + res + "NaoEncontradoError;\n"
               "use " + ctxNs + "\\Application\\Exceptions\\" + res + "Exception;\n"
               "use " + ctxNs + "\\Domain\\Entities\\" + res + "Entity;\n"
               "use " + ctxNs + "\\Infra\\Persistence\\Repositories\\" + res + "Repository;\n"
               "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
               "class " + action + res + "UseCase\n{\n"
               "    public function __construct(\n"
               "        private " + res + "Repository $" + repoVar + ",\n"
               "    ) {}\n\n"
               "    public function executar(IdVO $id, " + action + res + "Input $input): void\n    {\n"
               "        $registro = $this->" + repoVar + "->obterPorId($id);\n"
               "        if (!$registro) {\n"
               "            throw new " + res + "Exception(new " + res + "NaoEncontradoError());\n"
               "        }\n\n"
               "        $entity = " + res + "Entity::update(\n            id: $id,\n" + entityFields +
               "        );\n\n        $this->" + repoVar + "->alterar($entity);\n"
               "    }\n}\n";
    }

    // deletar
    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\UseCases;\n\n"
           "use " + ctxNs + "\\Application\\Errors\\" + res + "NaoEncontradoError;\n"
           "use " + ctxNs + "\\Application\\Exceptions\\" + res + "Exception;\n"
           "use " + ctxNs + "\\Infra\\Persistence\\Repositories\\" + res + "Repository;\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "class " + action + res + "UseCase\n{\n"
           "    public function __construct(\n"
           "        private " + res + "Repository $" + repoVar + ",\n"
           "    ) {}\n\n"
           "    public function executar(IdVO $id): void\n    {\n"
           "        $registro = $this->" + repoVar + "->obterPorId($id);\n"
           "        if (!$registro) {\n"
           "            throw new " + res + "Exception(new " + res + "NaoEncontradoError());\n"
           "        }\n\n        $this->" + repoVar + "->deletar($id);\n"
           "    }\n}\n";
}

std::string PhpTemplates::tplQuery(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";
    const std::string repoVar  = lcfirst(res) + "Repository";

    if (cfg.operation == "consultar") {
        std::string addArgs;
        for (const auto& f : cfg.fields)
            addArgs += "                " + f.name + ": "
                    + (endsWithVO(f.voType) ? "new " + f.voType + "($registro->" + f.name + ")"
                                            : "$registro->" + f.name)
                    + ",\n";

        return "<?php\n\ndeclare(strict_types=1);\n\n"
               "namespace " + ctxNs + "\\Application\\Queries;\n\n"
               "use " + ctxNs + "\\Application\\DTOs\\Outputs\\Consultar" + res + "sOutput;\n"
               "use " + ctxNs + "\\Infra\\Persistence\\Repositories\\" + res + "Repository;\n"
               "use " + sharedNs + "\\Application\\DTOs\\Inputs\\PaginacaoInput;\n"
               "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
               "class Consultar" + res + "sQuery\n{\n"
               "    public function __construct(\n"
               "        private " + res + "Repository $" + repoVar + ",\n"
               "    ) {}\n\n"
               "    public function executar(PaginacaoInput $input): Consultar" + res + "sOutput\n    {\n"
               "        $output    = new Consultar" + res + "sOutput();\n"
               "        $registros = $this->" + repoVar + "->obterListaPorPaginacao($input);\n\n"
               "        foreach ($registros as $registro) {\n"
               "            $output->adicionar" + res + "(\n" + addArgs +
               "            );\n        }\n\n"
               "        $output->page     = $registros->currentPage();\n"
               "        $output->lastPage = $registros->lastPage();\n"
               "        $output->total    = $registros->total();\n\n"
               "        return $output;\n    }\n}\n";
    }

    // detalhar
    std::string ctorArgs;
    for (const auto& f : cfg.fields)
        ctorArgs += "            " + f.name + ": "
                 + (endsWithVO(f.voType) ? "new " + f.voType + "($registro->" + f.name + ")"
                                         : "$registro->" + f.name)
                 + ",\n";

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\Queries;\n\n"
           "use " + ctxNs + "\\Application\\DTOs\\Outputs\\Detalhar" + res + "Output;\n"
           "use " + ctxNs + "\\Application\\Errors\\" + res + "NaoEncontradoError;\n"
           "use " + ctxNs + "\\Application\\Exceptions\\" + res + "Exception;\n"
           "use " + ctxNs + "\\Infra\\Persistence\\Repositories\\" + res + "Repository;\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "class Detalhar" + res + "Query\n{\n"
           "    public function __construct(\n"
           "        private " + res + "Repository $" + repoVar + ",\n"
           "    ) {}\n\n"
           "    public function executar(IdVO $id): Detalhar" + res + "Output\n    {\n"
           "        $registro = $this->" + repoVar + "->obterPorId($id);\n"
           "        if (!$registro) {\n"
           "            throw new " + res + "Exception(new " + res + "NaoEncontradoError());\n"
           "        }\n\n"
           "        return new Detalhar" + res + "Output(\n"
           "            id: new IdVO($registro->id),\n" + ctorArgs +
           "        );\n    }\n}\n";
}

std::string PhpTemplates::tplException(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\Exceptions;\n\n"
           "use " + sharedNs + "\\Application\\Exceptions\\BaseException;\n\n"
           "class " + res + "Exception extends BaseException {}\n";
}

std::string PhpTemplates::tplError(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::map<std::string, int> codes = {
        {"criar", 409}, {"alterar", 404}, {"deletar", 404},
        {"consultar", 404}, {"detalhar", 404}
    };
    const int code = codes.count(cfg.operation) ? codes.at(cfg.operation) : 404;

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Application\\Errors;\n\n"
           "use " + sharedNs + "\\Application\\Errors\\BaseError;\n\n"
           "class " + res + "NaoEncontradoError extends BaseError\n{\n"
           "    protected int $code = " + std::to_string(code) + ";\n"
           "    protected string $message = '" + res + " não encontrado';\n"
           "}\n";
}

std::string PhpTemplates::tplEntity(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::string props, params, assigns;
    for (const auto& f : cfg.fields) {
        const std::string nullable = f.nullable ? "?" : "";
        props   += "        public readonly " + nullable + f.voType + " $" + f.name + ",\n";
        params  += "        " + nullable + f.voType + " $" + f.name + ",\n";
        assigns += "            " + f.name + ": $" + f.name + ",\n";
    }

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Domain\\Entities;\n\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n\n"
           "class " + res + "Entity\n{\n"
           "    private function __construct(\n"
           "        public readonly ?IdVO $id,\n" + props +
           "    ) {}\n\n"
           "    public static function create(\n" + params +
           "    ): self {\n        return new self(\n            id: null,\n" + assigns +
           "        );\n    }\n\n"
           "    public static function update(\n        IdVO $id,\n" + params +
           "    ): self {\n        return new self(\n            id: $id,\n" + assigns +
           "        );\n    }\n}\n";
}

std::string PhpTemplates::tplRepository(const EndpointConfig& cfg)
{
    const std::string res      = ucfirst(cfg.resource);
    const std::string ctxNs    = "App\\Contexts\\" + cfg.context;
    const std::string sharedNs = "App\\Contexts\\Compartilhado\\Base";

    std::string assigns;
    for (const auto& f : cfg.fields)
        assigns += "            '" + f.name + "' => "
                + (endsWithVO(f.voType) ? "$entity->" + f.name + "->getValue()" : "$entity->" + f.name)
                + ",\n";

    return "<?php\n\ndeclare(strict_types=1);\n\n"
           "namespace " + ctxNs + "\\Infra\\Persistence\\Repositories;\n\n"
           "use " + ctxNs + "\\Domain\\Entities\\" + res + "Entity;\n"
           "use " + ctxNs + "\\Infra\\Persistence\\Models\\" + res + ";\n"
           "use " + sharedNs + "\\Application\\DTOs\\Inputs\\PaginacaoInput;\n"
           "use " + sharedNs + "\\Domain\\VOs\\IdVO;\n"
           "use Illuminate\\Pagination\\LengthAwarePaginator;\n\n"
           "class " + res + "Repository\n{\n"
           "    public function obterPorId(IdVO $id): ?object\n    {\n"
           "        return " + res + "::find($id->getValue());\n    }\n\n"
           "    public function obterListaPorPaginacao(PaginacaoInput $input): LengthAwarePaginator\n    {\n"
           "        return " + res + "::query()\n"
           "            ->paginate(perPage: $input->perPage, page: $input->page);\n    }\n\n"
           "    public function criar(" + res + "Entity $entity): IdVO\n    {\n"
           "        $model = " + res + "::create([\n" + assigns +
           "        ]);\n\n        return new IdVO($model->id);\n    }\n\n"
           "    public function alterar(" + res + "Entity $entity): void\n    {\n"
           "        " + res + "::where('id', $entity->id->getValue())->update([\n" + assigns +
           "        ]);\n    }\n\n"
           "    public function deletar(IdVO $id): void\n    {\n"
           "        " + res + "::destroy($id->getValue());\n    }\n}\n";
}

// ─── buildFiles ───────────────────────────────────────────────────────────────

std::vector<GeneratedFile> PhpTemplates::buildFiles(const EndpointConfig& cfg)
{
    namespace fs = std::filesystem;

    const std::string ctxBase = cfg.projectPath + "/back/app/Contexts/" + cfg.context;
    const std::string action  = ucfirst(cfg.operation);
    const std::string res     = ucfirst(cfg.resource);

    auto makeFile = [&](const std::string& rel) -> GeneratedFile {
        const std::string abs = ctxBase + "/" + rel;
        return { rel, abs, "", !Shared::fileExists(abs) };
    };

    std::vector<GeneratedFile> files;

    auto inp = makeFile("Application/DTOs/Inputs/" + action + res + "Input.php");
    inp.content = tplInputDTO(cfg);
    files.push_back(inp);

    if (cfg.operation == "consultar") {
        auto o = makeFile("Application/DTOs/Outputs/Consultar" + res + "sOutput.php");
        o.content = tplOutputPaginado(cfg);
        files.push_back(o);
        auto q = makeFile("Application/Queries/Consultar" + res + "sQuery.php");
        q.content = tplQuery(cfg);
        files.push_back(q);
    } else if (cfg.operation == "detalhar") {
        auto o = makeFile("Application/DTOs/Outputs/Detalhar" + res + "Output.php");
        o.content = tplOutputSimple(cfg);
        files.push_back(o);
        auto q = makeFile("Application/Queries/Detalhar" + res + "Query.php");
        q.content = tplQuery(cfg);
        files.push_back(q);
    } else {
        auto o = makeFile("Application/DTOs/Outputs/" + action + res + "Output.php");
        o.content = tplOutputSimple(cfg);
        files.push_back(o);
        auto uc = makeFile("Application/UseCases/" + action + res + "UseCase.php");
        uc.content = tplUseCase(cfg);
        files.push_back(uc);
    }

    auto exc = makeFile("Application/Exceptions/" + res + "Exception.php");
    exc.content = tplException(cfg);
    files.push_back(exc);

    auto err = makeFile("Application/Errors/" + res + "NaoEncontradoError.php");
    err.content = tplError(cfg);
    files.push_back(err);

    const std::string entityPath = "Domain/Entities/" + res + "Entity.php";
    if (!Shared::fileExists(ctxBase + "/" + entityPath)) {
        auto e = makeFile(entityPath);
        e.content = tplEntity(cfg);
        files.push_back(e);
    }

    const std::string repoPath = "Infra/Persistence/Repositories/" + res + "Repository.php";
    if (!Shared::fileExists(ctxBase + "/" + repoPath)) {
        auto r = makeFile(repoPath);
        r.content = tplRepository(cfg);
        files.push_back(r);
    }

    return files;
}

// ─── appendRoute ─────────────────────────────────────────────────────────────

void PhpTemplates::appendRoute(const EndpointConfig& cfg)
{
    const std::string routeFile = cfg.projectPath + "/back/app/Contexts/" + cfg.context
                                + "/Infra/Presentation/Routes/api.php";
    if (!Shared::fileExists(routeFile)) return;

    const std::map<std::string, std::pair<std::string, std::string>> routeMap = {
        {"consultar", {"get",    "/consultar"}},
        {"detalhar",  {"get",    "/detalhar/{id}"}},
        {"criar",     {"post",   "/criar"}},
        {"alterar",   {"put",    "/alterar/{id}"}},
        {"deletar",   {"delete", "/deletar/{id}"}},
    };
    if (!routeMap.count(cfg.operation)) return;

    const auto& [method, path] = routeMap.at(cfg.operation);
    const std::string res      = ucfirst(cfg.resource);
    const std::string line     = "Route::" + method + "('" + path + "', ["
                               + res + "Controller::class, '" + cfg.operation + "'])"
                               + "->name('" + cfg.operation + "');";

    std::ifstream rf(routeFile);
    std::string current((std::istreambuf_iterator<char>(rf)), std::istreambuf_iterator<char>());
    rf.close();

    if (current.find("'" + cfg.operation + "'") != std::string::npos) return;

    while (!current.empty() && (current.back() == '\n' || current.back() == '\r'))
        current.pop_back();

    Shared::writeFile(routeFile, current + "\n" + line + "\n");
}

} // namespace Infra
