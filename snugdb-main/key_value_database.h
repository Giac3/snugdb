#pragma once

#include <map>
#include <memory>
#include <string>
#include "collection.h"

class KeyValueDatabase {
public:
    KeyValueDatabase() = default;

    std::shared_ptr<Collection> create_collection(const std::string& name);
    std::shared_ptr<Collection> get_collection(const std::string& name) const;
    bool drop_collection(const std::string& collection_name);

    const std::map<std::string, std::shared_ptr<Collection>>& get_collections() const;

    std::string show_collections() const;

private:
    std::map<std::string, std::shared_ptr<Collection>> collections_;
    std::string active_database_; 
};