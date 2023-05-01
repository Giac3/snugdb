#include "surface.h"

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
    for (const auto& [key, value] : databases_) {
        ss << key << std::endl;
    }
    return ss.str();
}

void Surface::set_active_database(const std::string& name) {
    active_database_ = name;
}

std::string Surface::get_active_database() const {
    return active_database_;
}