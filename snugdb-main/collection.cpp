#include "collection.h"
#include <iostream>

const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m";

std::shared_ptr<Document> Collection::create_document(const std::string& name) {
    auto doc = std::make_shared<Document>();
    documents_[name] = doc;
    return doc;
}

std::shared_ptr<Document> Collection::get_document(const std::string& name) const {
    auto it = documents_.find(name);
    if (it != documents_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string Collection::show_documents() const {
    std::stringstream ss;
    if (documents_.empty()) {
        ss << "No collections." << std::endl;
        return ss.str();
    }

    size_t max_length = 0;
    for (const auto& [key, value] : documents_) {
        max_length = std::max(max_length, key.length());
    }

    // Print top border
    ss << YELLOW << "+" << "-";
    for (size_t i = 0; i < max_length; ++i) {
        ss << "-";
    }
    ss << "-" << "+" << RESET << std::endl;

    // Print items
    for (const auto& [key, value] : documents_) {
        ss << YELLOW << "|" << RESET << BLUE << ' ' << key;
        for (size_t i = 0; i < max_length - key.length(); ++i) {
            ss << ' ';
        }
        ss << ' ' << YELLOW << "|" << RESET << std::endl;
    }

    // Print bottom border
    ss << YELLOW << "+" << "-";
    for (size_t i = 0; i < max_length; ++i) {
        ss << "-";
    }
    ss << "-" << "+" << RESET << std::endl;

    return ss.str();
}



std::string Collection::show_document(const std::string& docname) const {
    auto doc = get_document(docname);
    if (doc) {
        return doc->show_data();
    }
    return "Document not found: " + docname;
}