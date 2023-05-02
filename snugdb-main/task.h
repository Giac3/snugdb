#pragma once

#include <string>
#include "document.h"

class Task {
public:
    enum class Operation {
        CreateDatabase,
        Add,
        Remove,
    };

    Task(Operation operation, const std::string& database_name)
        : operation_(operation), database_name_(database_name) {}

    Task(Operation operation, const std::shared_ptr<Document>& document, const std::string& key, const std::string& value = std::string())
        : operation_(operation), document_(document), key_(key), value_(value) {}

    Operation operation() const { return operation_; }
    std::string database_name() const { return database_name_; }
    std::shared_ptr<Document> document() const { return document_; }
    std::string key() const { return key_; }
    std::string value() const { return value_; }

private:
    Operation operation_;
    std::string database_name_;
    std::shared_ptr<Document> document_;
    std::string key_;
    std::string value_;
};