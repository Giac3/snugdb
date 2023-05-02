#include "surface.h"

#include <iostream>

const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m";

std::shared_ptr<KeyValueDatabase> Surface::create_database(const std::string& name) {
    auto db = std::make_shared<KeyValueDatabase>();
    databases_[name] = db;
    return db;
}

std::shared_ptr<KeyValueDatabase> Surface::get_database(const std::string& name) const {
    auto it = databases_.find(name);
    if (it != databases_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string Surface::show_databases() const {
    std::stringstream ss;
    if (databases_.empty()) {
        ss << "No databases." << std::endl;
        return ss.str();
    }

    size_t max_length = 0;
    for (const auto& [key, value] : databases_) {
        max_length = std::max(max_length, key.length());
    }

    // Print top border
    ss << YELLOW << "+" << "-";
    for (size_t i = 0; i < max_length; ++i) {
        ss << "-";
    }
    ss << "-" << "+" << RESET << std::endl;

    // Print items
    for (const auto& [key, value] : databases_) {
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

void Surface::set_active_database(const std::string& name) {
    active_database_ = name;
}

std::string Surface::get_active_database() const {
    return active_database_;
}

const std::map<std::string, std::shared_ptr<KeyValueDatabase>>& Surface::get_databases() const {
    return databases_;
}