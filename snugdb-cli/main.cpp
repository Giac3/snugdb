#include <iostream>
#include <sstream>
#include "../snugdb-main/surface.h"

const std::string RESET = "\033[0m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";
const std::string MAGENTA = "\033[35m";
const std::string YELLOW = "\033[33m";

std::string get_help_text() {
    std::stringstream help_text;

    help_text << GREEN << "Available commands:" << RESET << std::endl;
    help_text << YELLOW << "---------------------------------------------------------------------------------------------" << RESET << std::endl;
    help_text << MAGENTA << ".help" << RESET << "                   - Show this help message" << std::endl;
    help_text << MAGENTA << "create <db_name>" << RESET << "        - Create a new database with the given name" << std::endl;
    help_text << MAGENTA << "gotodb <db_name>" << RESET << "        - Switch to the specified database" << std::endl;
    help_text << MAGENTA << "createcol <col_name>" << RESET << "    - Create a new collection with the given name in the current database" << std::endl;
    help_text << MAGENTA << "addto <col_name> doc <doc_name>" << RESET << " - Add a new document with the given name to the specified collection" << std::endl;
    help_text << MAGENTA << "to <col.doc> add <key> <value_type> <value>" << RESET << " - Add a key-value pair to the specified document" << std::endl;
    help_text << MAGENTA << "showdb" << RESET << "                  - Show the list of collections in the current database" << std::endl;
    help_text << MAGENTA << "showcol <col_name>" << RESET << "      - Show the list of documents in the specified collection" << std::endl;
    help_text << MAGENTA << "showdoc <col.doc>" << RESET << "       - Show the data in the specified document" << std::endl;
    help_text << MAGENTA << "exit" << RESET << "                    - Exit the CLI" << std::endl;
    help_text << YELLOW << "---------------------------------------------------------------------------------------------" << RESET << std::endl;
    return help_text.str();
}

void create_database(Surface& surface, const std::string& dbname) {
    surface.create_database(dbname);
    std::cout << "Created database: " << dbname << std::endl;
}

void go_to_database(Surface& surface, const std::string& dbname, std::string& current_db) {
    std::string current_active_database = surface.get_active_database();
    if (current_active_database != dbname && surface.get_database(dbname)) {
        surface.set_active_database(dbname);
        current_db = dbname;
        std::cout << "Switched to database: " << dbname << std::endl;
    } else if (current_active_database == dbname) {
        std::cout << "Already in database: " << dbname << std::endl;
    } else {
        std::cout << "Database not found: " << dbname << std::endl;
    }
}

void create_collection(Surface& surface, const std::string& colname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        active_database->create_collection(colname); 
        std::cout << "Created collection: " << colname << std::endl;
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

void add_document(Surface& surface, const std::string& colname, const std::string& doc_type, const std::string& docname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        if (doc_type == "doc") {
            auto col = active_database->get_collection(colname);
            if (col) {
                col->create_document(docname);
                std::cout << "Added entry to " << colname << ": " << docname << std::endl;
            } else {
                std::cout << "Collection not found: " << colname << std::endl;
            }
        } else {
            std::cout << "Invalid document type: " << doc_type << ". Use 'doc' for adding a document." << std::endl;
        }
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

int main() {
    Surface surface;
    std::string input;
    std::string current_db;

        std::cout << CYAN << "Welcome to SnugDB CLI." << RESET << " Type '" << MAGENTA << ".help" << RESET << "' for help." << std::endl;

    while (true) {
        //std::cout << current_db << "> ";
        std::cout << MAGENTA << current_db << RESET << GREEN << " → " << RESET;
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::string command;
        iss >> command;
        
        if (command.empty()) {
            continue;
        } else if (command == "exit") {
            break;
        } else if (command == ".help") {
            std::cout << get_help_text();
        } else if (command == "create") {
            std::string dbname;
            iss >> dbname;
            create_database(surface, dbname);
        } else if (command == "gotodb") {
            std::string dbname;
            iss >> dbname;
            go_to_database(surface, dbname, current_db);
        } else if (command == "createcol") {
            std::string colname;
            iss >> colname;
            create_collection(surface,colname);
        } else if (command == "addto") {
                std::string colname, doc_type, docname;
                iss >> colname >> doc_type >> docname;
                add_document(surface, colname, doc_type, docname);
            } else if (command == "to") {
            std::string col_and_doc, cmd, key, value_type;
            iss >> col_and_doc >> cmd >> key >> value_type;
            size_t dot_pos = col_and_doc.find('.');
            if (dot_pos != std::string::npos) {
                std::string colname = col_and_doc.substr(0, dot_pos);
                std::string docname = col_and_doc.substr(dot_pos + 1);
                std::string active_database_name = surface.get_active_database();
                auto active_database = surface.get_database(active_database_name);
                auto col = active_database->get_collection(colname);
                auto entry = col->get_document(docname);
                if (entry && cmd == "add") {
                    std::string value_str;
                    std::getline(iss, value_str);
                    std::istringstream value_stream(value_str);
                    if (value_type == "int") {
                        int value;
                        value_stream >> value;
                        entry->set_value(key, value);
                    } else if (value_type == "double") {
                        double value;
                        value_stream >> value;
                        entry->set_value(key, value);
                    } else if (value_type == "string") {
                        std::string value;
                        value_stream >> value;
                        entry->set_value(key, value);
                    } else if (value_type == "bool") {
                        bool value;
                        value_stream >> std::boolalpha >> value;
                        entry->set_value(key, value);
                    } else {
                        std::cout << "Invalid value type: " << value_type << std::endl;
                    }
                } else {
                    std::cout << "Entry not found: " << docname << " in collection: " << colname << std::endl;
                }
            } else {
                std::cout << "Invalid input format. Use 'collection.document' format." << std::endl;
            }
        } else if (command == "showdb") {
            std::string active_database_name = surface.get_active_database();
            auto active_database = surface.get_database(active_database_name);
            if (active_database) {
                std::cout << "Collections in database: " << active_database_name << std::endl;
                std::cout << active_database->show_collections();
            } else {
                std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
            }
        } else if (command == "showcol") {
            std::string colname;
            iss >> colname;
            std::string active_database_name = surface.get_active_database();
            auto active_database = surface.get_database(active_database_name);
            if (active_database) {
                auto col = active_database->get_collection(colname);
                std::cout << "Documents in collection: " << colname << std::endl;
                std::cout << col->show_documents();
            } else {
                std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
            }
        } else if (command == "showdoc") {
            std::string col_and_doc;
            iss >> col_and_doc;
            size_t dot_pos = col_and_doc.find('.');
            if (dot_pos != std::string::npos) {
                std::string colname = col_and_doc.substr(0, dot_pos);
                std::string docname = col_and_doc.substr(dot_pos + 1);
                std::string active_database_name = surface.get_active_database();
                auto active_database = surface.get_database(active_database_name);
                if (active_database) {
                    auto col = active_database->get_collection(colname);
                    std::cout << "Data in document: " << docname << std::endl;
                    std::cout << col->show_document(docname);
                } else {
                    std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
                }
            } else {
                std::cout << "Invalid input format. Use 'collection.document' format." << std::endl;
            }
        }
    }

    return 0;
}