#pragma once
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <string>

namespace Shared {

namespace fs = std::filesystem;

inline void writeFile(const fs::path& path, const std::string& content)
{
    fs::create_directories(path.parent_path());
    std::ofstream f(path);
    if (f.is_open()) f << content;
}

inline bool fileExists(const fs::path& path) { return fs::exists(path); }

inline void mkdirs(std::initializer_list<fs::path> dirs)
{
    for (const auto& d : dirs) fs::create_directories(d);
}

} // namespace Shared
