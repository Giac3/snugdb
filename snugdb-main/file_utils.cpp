#include "file_utils.h"
#include <fstream>

void FileUtils::create_directory_if_not_exists(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }
}

void FileUtils::save_json_to_file(const std::string& file_path, const nlohmann::json& data) {
    std::ofstream file(file_path);
    if (file.is_open()) {
        file << data.dump(4); // Indent with 4 spaces for readability
        file.close();
    }
}

nlohmann::json FileUtils::load_json_from_file(const std::string& file_path) {
    nlohmann::json data;
    std::ifstream file(file_path);
    if (file.is_open()) {
        file >> data;
        file.close();
    }
    return data;
}