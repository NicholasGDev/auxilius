#pragma once
#include <sqlite3.h>
#include <string>

namespace Infra {

// Persistent key-value store at ~/.auxilius/auxilius.db
class Database {
public:
    static Database& instance();

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key, const std::string& def = "");
    std::string listJson();  // returns JSON array [{key,value}]

private:
    Database();
    ~Database();
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    sqlite3* db_ = nullptr;
    void exec(const char* sql);
};

} // namespace Infra
