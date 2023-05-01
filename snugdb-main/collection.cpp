#include "collection.h"

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
    for (const auto& [key, value] : documents_) {
        ss << key << std::endl;
    }
    return ss.str();
}

std::string Collection::show_document(const std::string& docname) const {
    auto doc = get_document(docname);
    if (doc) {
        return doc->show_data();
    }
    return "Document not found: " + docname;
}