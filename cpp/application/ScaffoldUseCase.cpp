#include "ScaffoldUseCase.hpp"
#include "../infra/scaffold/Scaffold.hpp"
#include "../shared/FileSystem.hpp"
#include <filesystem>
#include <iostream>

namespace Application {

void ScaffoldUseCase::generate(const std::string& outputPath)
{
    std::filesystem::path root(outputPath);
    std::cout << "  [1/3] back/     — Laravel DDD (14 contextos)\n";
    Infra::generateBackendScaffold(root);
    std::cout << "  [2/3] front/    — React + Giro DS (Vite)\n";
    Infra::generateFrontendScaffold(root);
    std::cout << "  [3/3] electron/ — Electron + React + Giro DS\n";
    Infra::generateElectronScaffold(root);

    // Root .gitignore
    Shared::writeFile(root / ".gitignore", R"(back/vendor/
back/.env
back/.env.*
!back/.env.example
back/storage/*.key
front/node_modules/
front/dist/
electron/node_modules/
electron/out/
electron/dist/
electron/release/
*.local
.DS_Store
Thumbs.db
)");
}

} // namespace Application
