#pragma once
#include "document.hpp"
#include <string>
#include <unordered_map>

namespace search_lib {
struct Relevance {
    std::size_t total = 0;
    std::unordered_map<std::string, size_t> stat;
};
Relevance count_relevance(const Document& document, const std::vector<std::string>& query);
}