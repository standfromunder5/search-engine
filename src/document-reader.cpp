#include "document-reader.hpp"
#include <OpenXLSX.hpp>
#include <nlohmann/json.hpp>
#include <rapidcsv.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace search_lib {
namespace {
void error(const std::filesystem::path& path, const std::string& message) {
    throw std::runtime_error(path.string() + ": " + message);
}
std::string to_lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<char>(c)));
    return s;
}
std::string cell_text(const OpenXLSX::XLCell& cell) {
    const auto& value = cell.value();
    switch (value.type()) {
        case OpenXLSX::XLValueType::String:
            return value.get<std::string>();
        case OpenXLSX::XLValueType::Integer:
            return std::to_string(value.get<std::int64_t>());
        case OpenXLSX::XLValueType::Float:
            return std::to_string(value.get<double>());
        case OpenXLSX::XLValueType::Boolean:
            return value.get<bool>()?"true":"false";
    }
}
}

std::vector<Document> CsvReader::read(const std::filesystem::path& path) const {
    try {
        const rapidcsv::Document csv(path.string(), rapidcsv::LabelParams(0, -1));
        const auto columns = csv.GetColumnNames();

        if (std::find(columns.begin(), columns.end(), "title") == columns.end())
            throw std::runtime_error("missing column 'title'");
        if (std::find(columns.begin(), columns.end(), "content") == columns.end())
            throw std::runtime_error("missing column 'content'");

        const auto titles = csv.GetColumn<std::string>("title");
        const auto contents = csv.GetColumn<std::string>("content");
        std::vector<Document> docs;
        docs.reserve(titles.size());
        for (std::size_t i = 0; i < titles.size() && i < contents.size(); ++i) {
            docs.emplace_back(path.string(), titles[i], contents[i]);
        }
        return docs;
    } catch (const std::exception& e) {
        error(path, e.what());
    }
}

std::vector<Document> JsonReader::read(const std::filesystem::path& path) const {
    try {
        std::ifstream in(path);
        const nlohmann::json root = nlohmann::json::parse(in);
        if (!root.is_array()) throw std::runtime_error("root element must be an array");

        std::vector<Document> docs;
        docs.reserve(root.size());
        for (const auto& item: root) {
            const bool check = item.is_object() && item.contains("title") && item["title"].is_string() && item.contains("content") && item["content"].is_string();
            if (!check) throw std::runtime_error("element must have 'title' and 'content' fields");
            docs.emplace_back(path.string(), item["title"].get<std::string>(), item["content"].get<std::string>());
        }
        return docs;
    } catch (const std::exception& e) {
        error(path, e.what());
    }
}

std::vector<Document> XlsxReader::read(const std::filesystem::path& path) const {
    try {
        OpenXLSX::XLDocument xlsx;
        xlsx.open(path.string());
        auto sheet = xlsx.workbook().worksheet(1);
        const std::uint32_t rows = sheet.rowCount();
        const std::uint16_t cols = sheet.columnCount();

        std::uint16_t title_col = 0;
        std::uint16_t content_col = 0;
        for (std::uint16_t c = 1; c <= cols; ++c) {
            const std::string name = cell_text(sheet.cell(1, c));
            if (name == "title") title_col = c;
            if (name == "content") content_col = c;
        }
        if (title_col == 0 || content_col == 0) throw std::runtime_error("header row must contain 'title' and 'content'");

        std::vector<Document> docs;
        for (std::uint32_t r = 2; r <= rows; ++r) {
            std::string title = cell_text(sheet.cell(r, title_col));
            std::string content = cell_text(sheet.cell(r, content_col));
            if (title.empty() && content.empty()) continue;
            docs.emplace_back(path.string(), std::move(title), std::move(content));
        }
        xlsx.close();
        return docs;
    } catch (const std::exception& e) {
        error(path, e.what());
    }
}

ReaderReg::ReaderReg() {
    add(".csv", std::make_unique<CsvReader>());
    add(".json", std::make_unique<JsonReader>());
    add(".xlsx", std::make_unique<XlsxReader>());
}

void ReaderReg::add(std::string extension, std::unique_ptr<DocumentReader> reader) {
    reader_table[to_lower(std::move(extension))] = std::move(reader);
}

const DocumentReader& ReaderReg::get(const std::filesystem::path& path) const {
    const std::string extension = to_lower(path.extension().string());
    const auto it = reader_table.find(extension);
    if (it == reader_table.end()) error(path, "unsupported file format '" + extension + "'");
    return *it->second;
}

std::vector<Document> read_docs(const std::filesystem::path& path, const ReaderReg& reg) {
    if (!std::filesystem::is_regular_file(path)) error(path, "file not found");
    return reg.get(path).read(path);
}

std::vector<Document> read_docs(const std::vector<std::filesystem::path>& paths, const ReaderReg& reg) {
    std::vector<Document> docs;
    for (const auto& path : paths) {
        auto cur = read_docs(path, reg);
        docs.insert(docs.end(), std::make_move_iterator(cur.begin()), std::make_move_iterator(cur.end()));
    }
    return docs;
}
}