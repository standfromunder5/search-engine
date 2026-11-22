#pragma once
#include "document.hpp"
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>

namespace search_lib {
class DocumentReader {
public:
    virtual ~DocumentReader() = default;
    virtual std::vector<Document> read(const std::filesystem::path& path) const = 0;
};

class CsvReader final: public DocumentReader {
public:
    std::vector<Document> read(const std::filesystem::path& path) const override;
};

class JsonReader final: public DocumentReader {
public:
    std::vector<Document> read(const std::filesystem::path& path) const override;
};

class XlsxReader final: public DocumentReader {
public:
    std::vector<Document> read(const std::filesystem::path& path) const override;
};

class ReaderReg {
public:
    ReaderReg();
    void add(std::string extension, std::unique_ptr<DocumentReader> reader);
    const DocumentReader& get(const std::filesystem::path& path) const;
private:
    std::unordered_map<std::string, std::unique_ptr<DocumentReader>> reader_table;
};

std::vector<Document> read_docs(const std::filesystem::path& path, const ReaderReg& reg);
std::vector<Document> read_docs(const std::vector<std::filesystem::path>& paths, const ReaderReg& reg);
}