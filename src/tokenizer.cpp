#include "tokenizer.hpp"
#include <unordered_set>
#include <utility>

namespace search_lib {
namespace {
bool alnum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
char lower(char c) {
    return (c >= 'A' && c <= 'Z')?static_cast<char>(c - 'A' + 'a'):c;
}
}

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string cur;
    for (const char c: text) {
        if (alnum(c)) cur.push_back(lower(c));
        else if (c == '\'') continue;
        else if (!cur.empty()) {
            tokens.push_back(std::move(cur));
            cur.clear();
        }
    }
    if (!cur.empty()) tokens.push_back(std::move(cur));

    std::vector<std::string> unique;
    std::unordered_set<std::string> s;
    for (auto& token: tokens) if (s.insert(token).second) unique.push_back(std::move(token));
    return unique;
}
}