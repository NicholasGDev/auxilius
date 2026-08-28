#include "Database.hpp"
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>
#include <string>

namespace Infra {

namespace fs = std::filesystem;

static std::string dbPath()
{
    const char* home = std::getenv("HOME");
    const std::string dir = std::string(home ? home : "/root") + "/.auxilius";
    fs::create_directories(dir);
    return dir + "/auxilius.db";
}

Database& Database::instance()
{
    static Database inst;
    return inst;
}

Database::Database()
{
    const std::string path = dbPath();
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK)
        throw std::runtime_error("Cannot open DB: " + path);

    exec("CREATE TABLE IF NOT EXISTS settings ("
         "  key        TEXT PRIMARY KEY,"
         "  value      TEXT NOT NULL DEFAULT '',"
         "  updated_at TEXT NOT NULL DEFAULT (datetime('now'))"
         ");");
}

Database::~Database()
{
    if (db_) sqlite3_close(db_);
}

void Database::exec(const char* sql)
{
    char* err = nullptr;
    sqlite3_exec(db_, sql, nullptr, nullptr, &err);
    if (err) {
        std::string msg = err;
        sqlite3_free(err);
        throw std::runtime_error("SQL error: " + msg);
    }
}

void Database::set(const std::string& key, const std::string& value)
{
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO settings(key, value, updated_at) VALUES(?,?,datetime('now'))"
        " ON CONFLICT(key) DO UPDATE SET value=excluded.value, updated_at=excluded.updated_at;";
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::string Database::get(const std::string& key, const std::string& def)
{
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, "SELECT value FROM settings WHERE key=?;", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    std::string result = def;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);
    return result;
}

std::string Database::listJson()
{
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, "SELECT key, value FROM settings ORDER BY key;", -1, &stmt, nullptr);
    std::string json = "[";
    bool first = true;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!first) json += ",";
        first = false;
        const std::string k = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const std::string v = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        // minimal JSON escaping
        auto esc = [](std::string s) {
            std::string r;
            for (char c : s) {
                if      (c == '"')  r += "\\\"";
                else if (c == '\\') r += "\\\\";
                else if (c == '\n') r += "\\n";
                else                r += c;
            }
            return r;
        };
        json += "{\"key\":\"" + esc(k) + "\",\"value\":\"" + esc(v) + "\"}";
    }
    sqlite3_finalize(stmt);
    return json + "]";
}

} // namespace Infra
