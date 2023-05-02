#include "key_value_database.h"
#include <iostream>

const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m";

std::shared_ptr<Collection> KeyValueDatabase::create_collection(const std::string& name) {
    auto col = std::make_shared<Collection>();
    collections_[name] = col;
    return col;
}

std::shared_ptr<Collection> KeyValueDatabase::get_collection(const std::string& name) const {
    auto it = collections_.find(name);
    if (it != collections_.end()) {
        return it->second;
    }
    return nullptr;
}

bool KeyValueDatabase::drop_collection(const std::string& collection_name) {
    auto it = collections_.find(collection_name);
    if (it != collections_.end()) {
        collections_.erase(it);
        return true;
    }
    return false;
}

std::string KeyValueDatabase::show_collections() const {
    std::stringstream ss;
    if (collections_.empty()) {
        ss << "No collections." << std::endl;
        return ss.str();
    }

    size_t max_length = 0;
    for (const auto& [key, value] : collections_) {
        max_length = std::max(max_length, key.length());
    }

    // Print top border
    ss << YELLOW << "+" << "-";
    for (size_t i = 0; i < max_length; ++i) {
        ss << "-";
    }
    ss << "-" << "+" << RESET << std::endl;

    // Print items
    for (const auto& [key, value] : collections_) {
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

const std::map<std::string, std::shared_ptr<Collection>>& KeyValueDatabase::get_collections() const {
    return collections_;
}