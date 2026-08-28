#pragma once
#include "../../domain/Field.hpp"
#include <string>
#include <vector>

namespace Infra {

using Domain::EndpointConfig;
using Domain::GeneratedFile;

class PhpTemplates {
public:
    static std::vector<GeneratedFile> buildFiles(const EndpointConfig& cfg);
    static void appendRoute(const EndpointConfig& cfg);

private:
    static std::string ucfirst(const std::string& s);
    static std::string lcfirst(const std::string& s);

    static std::string tplInputDTO(const EndpointConfig& cfg);
    static std::string tplOutputSimple(const EndpointConfig& cfg);
    static std::string tplOutputPaginado(const EndpointConfig& cfg);
    static std::string tplUseCase(const EndpointConfig& cfg);
    static std::string tplQuery(const EndpointConfig& cfg);
    static std::string tplException(const EndpointConfig& cfg);
    static std::string tplError(const EndpointConfig& cfg);
    static std::string tplEntity(const EndpointConfig& cfg);
    static std::string tplRepository(const EndpointConfig& cfg);
};

} // namespace Infra
