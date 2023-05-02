#pragma once

#include <map>
#include <memory>
#include <string>
#include "key_value_database.h"

class Surface {
public:
    Surface() = default;

    std::shared_ptr<KeyValueDatabase> create_database(const std::string& name);
    std::shared_ptr<KeyValueDatabase> get_database(const std::string& name) const;

    const std::map<std::string, std::shared_ptr<KeyValueDatabase>>& get_databases() const;

    std::string show_databases() const;

    void set_active_database(const std::string& name);
    std::string get_active_database() const;

private:
    std::map<std::string, std::shared_ptr<KeyValueDatabase>> databases_;
    std::string active_database_;
};