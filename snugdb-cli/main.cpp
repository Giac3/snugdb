#include <iostream>
#include <sstream>
#include "../snugdb-main/surface.h"
#include "../snugdb-main/file_utils.h"

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

// navigation functions

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

// Creation / Update functions

void create_database(Surface& surface, const std::string& dbname) {
    surface.create_database(dbname);
    std::cout << "Created database: " << dbname << std::endl;
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


void handle_to_commands(Surface& surface, std::istringstream& iss, const std::string& col_and_doc, const std::string& cmd, const std::string& key, const std::string& value_type) {
    size_t dot_pos = col_and_doc.find('.');
    if (dot_pos != std::string::npos) {
        std::string colname = col_and_doc.substr(0, dot_pos);
        std::string docname = col_and_doc.substr(dot_pos + 1);
        std::string active_database_name = surface.get_active_database();
        auto active_database = surface.get_database(active_database_name);
        auto col = active_database->get_collection(colname);
        if (col) {
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
            std::cout << "Collection not found: " << colname << std::endl;
        }
    } else {
        std::cout << "Invalid input format. Use 'collection.document' format." << std::endl;
    }
}


// Delete functions

void drop_collection(Surface& surface, const std::string& colname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        auto col = active_database->get_collection(colname);
        if (col) {
            bool dropped  = active_database->drop_collection(colname);

            if(dropped) {
                std::cout << "Dropped collection: " << colname << std::endl;
            } else {
                std::cout << "Could not drop collection:" << colname << std::endl;
            }
        } else {
            std::cout << "Collection does not exist" << std::endl;
        }
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

void drop_document(Surface& surface, const std::string& col_and_doc) {
    size_t dot_pos = col_and_doc.find('.');
    if (dot_pos != std::string::npos) {
        std::string colname = col_and_doc.substr(0, dot_pos);
        std::string docname = col_and_doc.substr(dot_pos + 1);
        std::string active_database_name = surface.get_active_database();
        auto active_database = surface.get_database(active_database_name);
        auto col = active_database->get_collection(colname);
        if(col) {
            auto entry = col->get_document(docname);
            if(entry) {

                bool dropped  = col->drop_document(docname);
                if(dropped) {
                    std::cout << "Dropped document: " << docname << std::endl;
                } else {
                    std::cout << "Could not drop document:" << docname << std::endl;
                }

            } else {
                std::cout << "Document not found: " << docname << std::endl;    
            }
        } else {
            std::cout << "Collection not found: " << colname << std::endl;    
        }
    } else {
        std::cout << "Invalid input format. Use 'collection.document' format." << std::endl;
    }
}

// General Read functions

void show_collection(Surface& surface, const std::string& colname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        auto col = active_database->get_collection(colname);
        if (col) {
            std::cout << "Documents in collection: " << colname << std::endl;
            std::cout << col->show_documents();
        } else {
            std::cout << "Collection does not exist" << std::endl;
        }
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

void show_document(Surface& surface, const std::string& col_and_doc) {
    size_t dot_pos = col_and_doc.find('.');
    if (dot_pos != std::string::npos) {
        std::string colname = col_and_doc.substr(0, dot_pos);
        std::string docname = col_and_doc.substr(dot_pos + 1);
        std::string active_database_name = surface.get_active_database();
        auto active_database = surface.get_database(active_database_name);
        if (active_database) {
            auto col = active_database->get_collection(colname);
            if (col) {
                auto doc = col->get_document(docname);
                if(doc) {
                    std::cout << "Data in document: " << docname << std::endl;
                    std::cout << col->show_document(docname);
                } else {
                    std::cout << "Document does not exist" << std::endl;    
                }
            } else {
                std::cout << "Collection does not exist" << std::endl;
            }
        } else {
            std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
        }
    } else {
        std::cout << "Invalid input format. Use 'collection.document' format." << std::endl;
    }
}

void show_database(Surface& surface) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        std::cout << CYAN << "Collections in database: " << RESET <<  active_database_name << std::endl;
        std::cout << active_database->show_collections();
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

// data persistance functions

void initialize_data_storage(Surface& surface, const std::string& data_directory) {
    // Create the main data directory if it does not already exist
    FileUtils::create_directory_if_not_exists(data_directory);

    // Loop through the snugdb_data folder and add any subfolders as databases
    for (const auto& db_entry : std::filesystem::directory_iterator(data_directory)) {
        if (db_entry.is_directory()) {
            std::string db_name = db_entry.path().filename().string();
            auto db_object = surface.create_database(db_name);

            // Loop through each database folder and add subfolders as collections
            std::string db_path = data_directory + "/" + db_name;
            for (const auto& coll_entry : std::filesystem::directory_iterator(db_path)) {
                if (coll_entry.is_directory()) {
                    std::string coll_name = coll_entry.path().filename().string();
                    db_object->create_collection(coll_name);
                }
            }
        }
    }
}

int main() {
    Surface surface;
    std::string input;
    std::string current_db;
    std::string data_directory = "snugdb_data";
    initialize_data_storage(surface, data_directory);
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
            std::cout << MAGENTA << "Bye Bye 👋" << RESET << std::endl;
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
        } else if (command == "dropcol") {
            std::string colname;
            iss >> colname;
            drop_collection(surface,colname);
        } else if (command == "addto") {
                std::string colname, doc_type, docname;
                iss >> colname >> doc_type >> docname;
                add_document(surface, colname, doc_type, docname);
        } else if (command == "to") {
            std::string col_and_doc, cmd, key, value_type;
            iss >> col_and_doc >> cmd >> key >> value_type;
            handle_to_commands(surface, iss, col_and_doc, cmd, key, value_type);
        } else if (command == "dropdoc") {
            std::string col_and_doc;
            iss >> col_and_doc;
            drop_document(surface,col_and_doc);
        } else if (command == "showdb") {
            show_database(surface);
        } else if (command == "showcol") {
            std::string colname;
            iss >> colname;
            show_collection(surface, colname);
        } else if (command == "showdoc") {
            std::string col_and_doc;
            iss >> col_and_doc;
            show_document(surface, col_and_doc);
        }
    }

    return 0;
}