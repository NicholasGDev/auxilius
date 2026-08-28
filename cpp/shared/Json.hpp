#pragma once
#include <cstdio>
#include <string>

namespace Shared {

inline std::string jsonEscape(const std::string& s)
{
    std::string r;
    r.reserve(s.size() + 16);
    for (unsigned char c : s) {
        switch (c) {
        case '"':  r += "\\\""; break;
        case '\\': r += "\\\\"; break;
        case '\n': r += "\\n";  break;
        case '\r': r += "\\r";  break;
        case '\t': r += "\\t";  break;
        default:
            if (c < 0x20) {
                char buf[8];
                snprintf(buf, sizeof(buf), "\\u%04x", c);
                r += buf;
            } else {
                r += static_cast<char>(c);
            }
        }
    }
    return r;
}

inline std::string jstr(const std::string& s) { return "\"" + jsonEscape(s) + "\""; }
inline std::string jbool(bool b)               { return b ? "true" : "false"; }

} // namespace Shared
