#include "document.hpp"
#include <string>

namespace search_lib {
    Document::Document(std::string p, std::string t, std::string c): path(std::move(p)), title(std::move(t)), content(std::move(c)) {}
}