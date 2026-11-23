#include "relevance.hpp"
#include "tokenizer.hpp"
#include <unordered_map>

namespace search_lib {
Relevance count_relevance(const Document& document, const std::vector<std::string>& query) {
    Relevance res;
    res.stat.reserve(query.size());
    for (const auto& token: tokenize(document.get_content())) {
        res.stat[token]++;
        res.total++;
    }
    return res;
}
}