#pragma once

#include <map>
#include <string>
#include <variant>
#include <sstream> 

class Document {
public:
    Document() = default;

    void set_value(const std::string& key, const std::variant<int, double, std::string, bool>& value);
    bool remove_value(const std::string& key);

    std::string show_data() const;

private:
    std::map<std::string, std::variant<int, double, std::string, bool>> data_;
};