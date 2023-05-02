#pragma once

#include <map>
#include <string>
#include <variant>
#include <sstream> 
#include "json.hpp"

class Document {
public:
    Document() = default;

    void set_value(const std::string& key, const nlohmann::json& value);
    bool remove_value(const std::string& key);
    void set_data(const nlohmann::json& data) { data_ = data; }
    std::string show_data() const;

private:
    std::map<std::string, nlohmann::json> data_;
};