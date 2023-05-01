#include "key_value_database.h"

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

std::string KeyValueDatabase::show_collections() const {
    std::stringstream ss;
    for (const auto& [key, value] : collections_) {
        ss << key << std::endl;
    }
    return ss.str();
}
