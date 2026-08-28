#include "EndpointUseCase.hpp"
#include "../infra/templates/PhpTemplates.hpp"
#include "../shared/FileSystem.hpp"
#include "../shared/Json.hpp"
#include <fstream>

namespace Application {

std::string EndpointUseCase::preview(const EndpointConfig& cfg)
{
    const auto files = Infra::PhpTemplates::buildFiles(cfg);

    std::string json = "[";
    for (size_t i = 0; i < files.size(); ++i) {
        if (i > 0) json += ",";
        json += "{"
                "\"relativePath\":" + Shared::jstr(files[i].relativePath) + ","
                "\"absolutePath\":" + Shared::jstr(files[i].absolutePath) + ","
                "\"content\":"      + Shared::jstr(files[i].content)      + ","
                "\"isNew\":"        + Shared::jbool(files[i].isNew)        +
                "}";
    }
    json += "]";
    return json;
}

std::string EndpointUseCase::generate(const EndpointConfig& cfg)
{
    const auto files = Infra::PhpTemplates::buildFiles(cfg);

    std::vector<std::string> written, skipped;
    for (const auto& f : files) {
        if (!f.isNew) { skipped.push_back(f.relativePath); continue; }
        Shared::writeFile(f.absolutePath, f.content);
        written.push_back(f.relativePath);
    }

    Infra::PhpTemplates::appendRoute(cfg);
    written.push_back("Infra/Presentation/Routes/api.php (rota adicionada)");

    auto toJsonArray = [](const std::vector<std::string>& v) {
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i > 0) s += ",";
            s += Shared::jstr(v[i]);
        }
        return s + "]";
    };

    return "{\"success\":true,"
           "\"written\":" + toJsonArray(written) + ","
           "\"skipped\":" + toJsonArray(skipped) + "}";
}

} // namespace Application
