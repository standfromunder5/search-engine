#pragma once
#include <string>

namespace search_lib {
class Document {
    std::string path;
    std::string title;
    std::string content;
public:
    const std::string& get_path() const { return path; }
    const std::string& get_title() const { return title; }
    const std::string& get_content() const { return content; }
    Document(std::string path, std::string title, std::string content);
};
}