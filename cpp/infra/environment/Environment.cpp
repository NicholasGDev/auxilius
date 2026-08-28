#include "Environment.hpp"
#include "../../shared/Json.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace Infra {

std::string Environment::runCommand(const std::string& cmd)
{
    std::array<char, 512> buf{};
    std::string result;
    FILE* pipe = popen((cmd + " 2>&1").c_str(), "r");
    if (!pipe) return "";
    while (fgets(buf.data(), buf.size(), pipe) != nullptr)
        result += buf.data();
    pclose(pipe);
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
        result.pop_back();
    return result;
}

std::string Environment::readFile(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

void Environment::writeFile(const std::string& path, const std::string& content)
{
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream f(path);
    if (f.is_open()) f << content;
}

std::string Environment::check(const std::string& projectPath)
{
    using Shared::jstr;
    using Shared::jbool;

    const std::string gitVer  = runCommand("git --version");
    const bool hasGit         = gitVer.find("git version") != std::string::npos;

    const std::string nvmVer  = runCommand("bash -c 'source ~/.nvm/nvm.sh 2>/dev/null && nvm --version'");
    const bool hasNvm         = !nvmVer.empty() && nvmVer.find("not found") == std::string::npos && nvmVer.size() > 1;

    const std::string node20  = runCommand("bash -c 'source ~/.nvm/nvm.sh 2>/dev/null && nvm run 20 --version 2>&1 | tail -1'");
    const bool hasNode20      = node20.find("v20") != std::string::npos;

    const std::string node19  = runCommand("bash -c 'source ~/.nvm/nvm.sh 2>/dev/null && nvm run 19 --version 2>&1 | tail -1'");
    const bool hasNode19      = node19.find("v19") != std::string::npos;

    const std::string wslConf = readFile("/etc/wsl.conf");
    const bool rootDefault    = wslConf.find("default=root") != std::string::npos;

    const bool projExists     = !projectPath.empty() && std::filesystem::exists(projectPath);

    return "{"
           "\"git\":{\"ok\":" + jbool(hasGit) + ",\"version\":" + jstr(gitVer) + "},"
           "\"nvm\":{\"ok\":" + jbool(hasNvm) + ",\"version\":" + jstr(nvmVer) + "},"
           "\"node20\":{\"ok\":" + jbool(hasNode20) + ",\"version\":" + jstr(node20) + "},"
           "\"node19\":{\"ok\":" + jbool(hasNode19) + ",\"version\":" + jstr(node19) + "},"
           "\"rootDefault\":" + jbool(rootDefault) + ","
           "\"projectExists\":" + jbool(projExists) + ","
           "\"projectPath\":" + jstr(projectPath) +
           "}";
}

std::string Environment::setupWsl()
{
    const std::string path = "/etc/wsl.conf";
    std::string conf       = readFile(path);

    if (conf.find("[user]") == std::string::npos) {
        conf += "\n[user]\ndefault=root\n";
    } else if (conf.find("default=root") == std::string::npos) {
        const size_t pos = conf.find("[user]");
        conf.insert(pos + std::string("[user]").size() + 1, "default=root\n");
    }

    try {
        writeFile(path, conf);
        return "{\"success\":true,\"message\":\"WSL configurado. Reinicie: wsl --shutdown\"}";
    } catch (...) {
        return "{\"success\":false,\"error\":\"Falha ao escrever /etc/wsl.conf\"}";
    }
}

std::string Environment::setupNvm()
{
    const std::string script =
        "export NVM_DIR=\"$HOME/.nvm\"; "
        "if [ ! -s \"$NVM_DIR/nvm.sh\" ]; then "
        "  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash; "
        "fi; "
        "[ -s \"$NVM_DIR/nvm.sh\" ] && . \"$NVM_DIR/nvm.sh\"; "
        "nvm install 19; nvm install 20; nvm alias default 20; nvm use 20; "
        "echo \"Node $(node -v) configurado\"";

    const std::string output = runCommand("bash -c '" + script + "'");
    const bool success       = output.find("configurado") != std::string::npos;

    return "{\"success\":" + Shared::jbool(success) + ",\"stdout\":" + Shared::jstr(output) + "}";
}

std::string Environment::cloneProject(const std::string& targetDir)
{
    if (std::filesystem::exists(targetDir))
        return "{\"success\":false,\"error\":\"Projeto já existe em " + targetDir + "\"}";

    std::filesystem::create_directories(std::filesystem::path(targetDir).parent_path());

    const std::string output = runCommand(
        "git clone https://gitlab.zanthus.com.br/web/zeus-retail-evolution.git \"" + targetDir + "\"");
    const bool success = std::filesystem::exists(targetDir);

    return "{\"success\":" + Shared::jbool(success) + ",\"stdout\":" + Shared::jstr(output) + "}";
}

std::string Environment::configureGit(const std::string& name, const std::string& email)
{
    runCommand("git config --global user.name \"" + name + "\"");
    runCommand("git config --global user.email \"" + email + "\"");
    return "{\"success\":true,\"message\":\"Git configurado: " + name + " <" + email + ">\"}";
}

} // namespace Infra
