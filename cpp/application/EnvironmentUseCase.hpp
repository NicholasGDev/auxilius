#pragma once
#include <string>

namespace Application {

class EnvironmentUseCase {
public:
    static std::string check(const std::string& projectPath);
    static std::string setupWsl();
    static std::string setupNvm();
    static std::string cloneProject(const std::string& dir);
    static std::string configureGit(const std::string& name, const std::string& email);
};

} // namespace Application
