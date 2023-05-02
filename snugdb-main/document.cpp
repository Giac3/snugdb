#include "document.h"
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m";

void Document::set_value(const std::string& key, const std::variant<int, double, std::string, bool>& value) {
    data_[key] = value;
}

std::string Document::show_data() const {
    std::stringstream ss;
    if (data_.empty()) {
        ss << "No data." << std::endl;
        return ss.str();
    }

    size_t max_length = 0;
    for (const auto& [key, value] : data_) {
        std::stringstream temp_ss;
        temp_ss << key << ": ";
        std::visit([&](const auto& v) {
            temp_ss << std::boolalpha << v;
        }, value);
        max_length = std::max(max_length, temp_ss.str().length());
    }

    // Print top border
    ss << YELLOW << "+" << "-";
    for (size_t i = 0; i < max_length; ++i) {
        ss << "-";
    }
    ss << "-" << "+" << RESET << std::endl;

    // Print items
    for (const auto& [key, value] : data_) {
        std::stringstream temp_ss;
        temp_ss << key << ": ";
        std::visit([&](const auto& v) {
            temp_ss << std::boolalpha << v;
        }, value);

        ss << YELLOW << "|" << RESET << BLUE << ' ' << temp_ss.str();
        for (size_t i = 0; i < max_length - temp_ss.str().length(); ++i) {
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