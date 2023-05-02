#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <filesystem>
#include "json.hpp"

class FileUtils {
public:
    static void create_directory_if_not_exists(const std::string& path);
    static void save_json_to_file(const std::string& file_path, const nlohmann::json& data);
    static nlohmann::json load_json_from_file(const std::string& file_path);
};

#endif // FILE_UTILS_H