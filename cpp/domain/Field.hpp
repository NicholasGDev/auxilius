#pragma once
#include <string>
#include <vector>

namespace Domain {

struct Field {
    std::string name;
    std::string phpType;  // string|int|float|bool
    std::string voType;   // IdVO|EmailVO|string|int|float|bool
    bool nullable = false;
};

struct EndpointConfig {
    std::string context;
    std::string resource;
    std::string operation;  // criar|alterar|deletar|consultar|detalhar
    std::string projectPath;
    std::vector<Field> fields;
};

struct GeneratedFile {
    std::string relativePath;
    std::string absolutePath;
    std::string content;
    bool isNew = true;
};

} // namespace Domain
