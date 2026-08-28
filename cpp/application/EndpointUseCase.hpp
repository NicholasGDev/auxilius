#pragma once
#include "../domain/Field.hpp"
#include <string>

namespace Application {

using Domain::EndpointConfig;

class EndpointUseCase {
public:
    // Returns JSON array of GeneratedFile objects
    static std::string preview(const EndpointConfig& cfg);
    // Writes files to disk; returns JSON result {success, written[], skipped[]}
    static std::string generate(const EndpointConfig& cfg);
};

} // namespace Application
