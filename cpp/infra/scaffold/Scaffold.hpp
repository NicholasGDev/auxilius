#pragma once
#include <filesystem>

namespace Infra {
void generateBackendScaffold(const std::filesystem::path& root);
void generateFrontendScaffold(const std::filesystem::path& root);
void generateElectronScaffold(const std::filesystem::path& root);
} // namespace Infra
