#pragma once

#include <map>
#include <memory>
#include <string>
#include <sstream> 
#include "document.h"

class Document; 

class Collection {
public:
    Collection() = default;

    std::shared_ptr<Document> create_document(const std::string& name);
    std::shared_ptr<Document> get_document(const std::string& name) const;

    std::string show_documents() const;
    std::string show_document(const std::string& docname) const;

private:
    std::map<std::string, std::shared_ptr<Document>> documents_;
};