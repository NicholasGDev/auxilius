#pragma once
#include <string>

namespace Infra {

class Environment {
public:
    static std::string check(const std::string& projectPath);
    static std::string setupWsl();
    static std::string setupNvm();
    static std::string cloneProject(const std::string& targetDir);
    static std::string configureGit(const std::string& name, const std::string& email);

private:
    static std::string runCommand(const std::string& cmd);
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
};

} // namespace Infra
