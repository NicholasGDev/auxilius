#include "EnvironmentUseCase.hpp"
#include "../infra/environment/Environment.hpp"

namespace Application {

std::string EnvironmentUseCase::check(const std::string& projectPath) {
    return Infra::Environment::check(projectPath);
}
std::string EnvironmentUseCase::setupWsl() {
    return Infra::Environment::setupWsl();
}
std::string EnvironmentUseCase::setupNvm() {
    return Infra::Environment::setupNvm();
}
std::string EnvironmentUseCase::cloneProject(const std::string& dir) {
    return Infra::Environment::cloneProject(dir);
}
std::string EnvironmentUseCase::configureGit(const std::string& name, const std::string& email) {
    return Infra::Environment::configureGit(name, email);
}

} // namespace Application
