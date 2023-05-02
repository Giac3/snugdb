#pragma once

#include <string>
#include "document.h"

class Task {
public:
    enum class Operation {
        CreateDatabase,
        CreateCollection,
        SaveDocument,
        Add,
        Remove,
    };

    Task(Operation operation, const std::string& database_name, const std::string& collection_name = std::string())
        : operation_(operation), database_name_(database_name), collection_name_(collection_name) {}

    Task(Operation operation, const std::shared_ptr<Document>& document, const std::string& key, const std::string& value = std::string())
        : operation_(operation), document_(document), key_(key), value_(value) {}

    Task(Operation operation, const std::string& database_name, const std::string& collection_name, const std::string& document_name, const nlohmann::json& document_data)
        : operation_(operation), database_name_(database_name), collection_name_(collection_name), document_name_(document_name), document_data_(document_data) {}
        
    Task(Operation operation, const std::string& database_name, const std::string& collection_name, const std::string& document_name)
    : operation_(operation), database_name_(database_name), collection_name_(collection_name), document_name_(document_name) {}

    Operation operation() const { return operation_; }
    std::string database_name() const { return database_name_; }
    std::string collection_name() const { return collection_name_; }
    std::shared_ptr<Document> document() const { return document_; }
    std::string key() const { return key_; }
    std::string value() const { return value_; }
    std::string document_name() const { return document_name_; } // Getter for document_name_
    nlohmann::json document_data() const { return document_data_; } // Getter for document_data_

private:
    Operation operation_;
    std::string database_name_;
    std::string collection_name_;
    std::shared_ptr<Document> document_;
    std::string key_;
    std::string value_;
    std::string document_name_; // Add new members for document_name and document_data
    nlohmann::json document_data_;
};