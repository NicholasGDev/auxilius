// scaffold_zeus — CLI entry point
// Usage:
//   scaffold_zeus [scaffold] [output_dir]
//   scaffold_zeus endpoint --context=X --resource=X --operation=X --fields=f:t,... --project=X [--preview|--generate]
//   scaffold_zeus env check [--project=X]
//   scaffold_zeus env setup-wsl
//   scaffold_zeus env setup-nvm
//   scaffold_zeus env clone-project [--dir=X]
//   scaffold_zeus env git-config --name=X --email=X

#include "application/ScaffoldUseCase.hpp"
#include "application/EndpointUseCase.hpp"
#include "application/EnvironmentUseCase.hpp"
#include "domain/Field.hpp"
#include "infra/database/Database.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// ─── Helpers ─────────────────────────────────────────────────────────────────

static std::map<std::string, std::string> parseArgs(int argc, char* argv[], int start)
{
    std::map<std::string, std::string> m;
    for (int i = start; i < argc; ++i) {
        std::string a = argv[i];
        if (a.size() > 2 && a[0] == '-' && a[1] == '-') {
            const auto eq = a.find('=');
            if (eq != std::string::npos)
                m[a.substr(2, eq - 2)] = a.substr(eq + 1);
            else
                m[a.substr(2)] = "true";
        }
    }
    return m;
}

static std::string suggestVO(const std::string& name, const std::string& phpType)
{
    std::string low = name;
    std::transform(low.begin(), low.end(), low.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (low == "id" || (low.size() > 2 && low.substr(low.size() - 2) == "id")) return "IdVO";
    if (low.find("email")    != std::string::npos) return "EmailVO";
    if (low.find("cpf")      != std::string::npos) return "CPFVO";
    if (low.find("cnpj")     != std::string::npos) return "CNPJVO";
    if (low.find("telefone") != std::string::npos ||
        low.find("fone")     != std::string::npos ||
        low.find("phone")    != std::string::npos) return "PhoneVO";
    if (low.find("cep")      != std::string::npos) return "CepVO";
    if (low.find("preco")    != std::string::npos ||
        low.find("valor")    != std::string::npos ||
        low.find("total")    != std::string::npos) return "MoneyVO";
    if (low.find("decimal")  != std::string::npos) return "DecimalVO";

    if (phpType == "int")   return "int";
    if (phpType == "float") return "float";
    if (phpType == "bool")  return "bool";
    return "string";
}

// Parses "nome:string,email:string?,cpf:string" into Field vector
static std::vector<Domain::Field> parseFields(const std::string& s)
{
    std::vector<Domain::Field> fields;
    if (s.empty()) return fields;

    std::istringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (token.empty()) continue;
        Domain::Field f;
        f.nullable = !token.empty() && token.back() == '?';
        if (f.nullable) token.pop_back();

        const auto colon = token.find(':');
        if (colon == std::string::npos) {
            f.name    = token;
            f.phpType = "string";
        } else {
            f.name    = token.substr(0, colon);
            f.phpType = token.substr(colon + 1);
        }
        f.voType = suggestVO(f.name, f.phpType);
        fields.push_back(f);
    }
    return fields;
}

static const char* homeDir()
{
    const char* h = std::getenv("HOME");
    return h ? h : "/root";
}

// ─── main ────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Gerando scaffold em: .\n";
        Application::ScaffoldUseCase::generate(".");
        return 0;
    }

    const std::string cmd = argv[1];

    // ── scaffold ──────────────────────────────────────────────────────────────
    if (cmd == "scaffold") {
        const std::string path = (argc >= 3) ? argv[2] : ".";
        std::cout << "Gerando scaffold em: " << path << "\n";
        Application::ScaffoldUseCase::generate(path);
        return 0;
    }

    // ── endpoint ──────────────────────────────────────────────────────────────
    if (cmd == "endpoint") {
        const auto args = parseArgs(argc, argv, 2);

        Domain::EndpointConfig cfg;
        cfg.context     = args.count("context")   ? args.at("context")   : "";
        cfg.resource    = args.count("resource")  ? args.at("resource")  : "";
        cfg.operation   = args.count("operation") ? args.at("operation") : "";
        cfg.projectPath = args.count("project")   ? args.at("project")   : ".";
        cfg.fields      = parseFields(args.count("fields") ? args.at("fields") : "");

        if (cfg.context.empty() || cfg.resource.empty() || cfg.operation.empty()) {
            std::cerr << "{\"error\":\"--context, --resource e --operation são obrigatórios\"}\n";
            return 1;
        }

        if (args.count("preview")) {
            std::cout << Application::EndpointUseCase::preview(cfg) << "\n";
        } else if (args.count("generate")) {
            std::cout << Application::EndpointUseCase::generate(cfg) << "\n";
        } else {
            std::cerr << "{\"error\":\"use --preview ou --generate\"}\n";
            return 1;
        }
        return 0;
    }

    // ── env ───────────────────────────────────────────────────────────────────
    if (cmd == "env") {
        if (argc < 3) {
            std::cerr << "{\"error\":\"subcomando de env obrigatório\"}\n";
            return 1;
        }
        const std::string sub  = argv[2];
        const auto args        = parseArgs(argc, argv, 3);

        if (sub == "check") {
            const std::string project = args.count("project")
                ? args.at("project")
                : std::string(homeDir()) + "/projects/zeus-retail-evolution";
            std::cout << Application::EnvironmentUseCase::check(project) << "\n";

        } else if (sub == "setup-wsl") {
            std::cout << Application::EnvironmentUseCase::setupWsl() << "\n";

        } else if (sub == "setup-nvm") {
            std::cout << Application::EnvironmentUseCase::setupNvm() << "\n";

        } else if (sub == "clone-project") {
            const std::string dir = args.count("dir")
                ? args.at("dir")
                : std::string(homeDir()) + "/projects/zeus-retail-evolution";
            std::cout << Application::EnvironmentUseCase::cloneProject(dir) << "\n";

        } else if (sub == "git-config") {
            const std::string name  = args.count("name")  ? args.at("name")  : "";
            const std::string email = args.count("email") ? args.at("email") : "";
            std::cout << Application::EnvironmentUseCase::configureGit(name, email) << "\n";

        } else {
            std::cerr << "{\"error\":\"subcomando desconhecido: " + sub + "\"}\n";
            return 1;
        }
        return 0;
    }

    // ── db ────────────────────────────────────────────────────────────────────
    if (cmd == "db") {
        if (argc < 3) {
            std::cerr << "{\"error\":\"subcomando db: get <key> | set <key> <value> | list\"}\n";
            return 1;
        }
        const std::string sub = argv[2];
        auto& db = Infra::Database::instance();

        if (sub == "get") {
            if (argc < 4) { std::cerr << "{\"error\":\"db get requer <key>\"}\n"; return 1; }
            const std::string val = db.get(argv[3]);
            std::cout << "{\"key\":\"" << argv[3] << "\",\"value\":\"" << val << "\"}\n";

        } else if (sub == "set") {
            if (argc < 5) { std::cerr << "{\"error\":\"db set requer <key> <value>\"}\n"; return 1; }
            db.set(argv[3], argv[4]);
            std::cout << "{\"ok\":true,\"key\":\"" << argv[3] << "\"}\n";

        } else if (sub == "list") {
            std::cout << db.listJson() << "\n";

        } else {
            std::cerr << "{\"error\":\"subcomando db desconhecido: " + sub + "\"}\n";
            return 1;
        }
        return 0;
    }

    // ── fallback: treat arg as output path for scaffold ───────────────────────
    Application::ScaffoldUseCase::generate(argv[1]);
    return 0;
}