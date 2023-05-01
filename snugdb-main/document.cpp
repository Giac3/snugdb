#include "document.h"

void Document::set_value(const std::string& key, const std::variant<int, double, std::string, bool>& value) {
    data_[key] = value;
}

std::string Document::show_data() const {
    std::stringstream ss;
    for (const auto& [key, value] : data_) {
        ss << key << ": ";
        std::visit([&](const auto& v) {
            ss << std::boolalpha << v; 
        }, value);
        ss << std::endl;
    }
    return ss.str();
}