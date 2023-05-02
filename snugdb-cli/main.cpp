#include <iostream>
#include <sstream>
#include "../snugdb-main/surface.h"
#include "../snugdb-main/file_utils.h"
#include "../snugdb-main/task.h"
#include "../snugdb-main/task_queue.h"
#include "json.hpp"
#include <fstream>

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

void create_database(Surface& surface, TaskQueue& task_queue, const std::string& dbname) {
    surface.create_database(dbname);
    std::cout << "Created database: " << dbname << std::endl;

    // Add a task to the task queue
    Task create_db_task(Task::Operation::CreateDatabase, dbname);
    task_queue.add_task(create_db_task);
}


void create_collection(Surface& surface, TaskQueue& task_queue, const std::string& colname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        active_database->create_collection(colname); 
        std::cout << "Created collection: " << colname << std::endl;

        // Add a task to the task queue
        Task create_col_task(Task::Operation::CreateCollection, active_database_name, colname);
        task_queue.add_task(create_col_task);
    } else {
        std::cout << "No active database. Use 'create' and 'gotodb' commands to create and select a database." << std::endl;
    }
}

void add_document(Surface& surface,TaskQueue& task_queue, const std::string& colname, const std::string& doc_type, const std::string& docname) {
    std::string active_database_name = surface.get_active_database();
    auto active_database = surface.get_database(active_database_name);
    if (active_database) {
        if (doc_type == "doc") {
            auto col = active_database->get_collection(colname);
            if (col) {
                if(col->get_document(docname)) {
                    std::cout << "Document already exists" << std::endl;
                } else {
                    col->create_document(docname);
                    std::cout << "Added document to " << colname << ": " << docname << std::endl;
                    Task save_document_task(Task::Operation::SaveDocument, active_database_name, colname, docname, nlohmann::json::object());
                    task_queue.add_task(save_document_task);
                }
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
            if (entry ) {
                 if (entry && cmd == "add") {
                    nlohmann::json value;
                    bool valid_value_type = true;
                    if (value_type == "int") {
                        int int_value;
                        iss >> int_value;
                        value = int_value;
                    } else if (value_type == "double") {
                        double double_value;
                        iss >> double_value;
                        value = double_value;
                    } else if (value_type == "string") {
                        std::string string_value;
                        std::getline(iss, string_value);
                        value = string_value;
                    } else if (value_type == "bool") {
                        std::string bool_value;
                        iss >> bool_value;
                        value = (bool_value == "true");
                    } else if (value_type == "list" || value_type == "object") {
                        std::string value_str;
                        std::getline(iss, value_str);
                        try {
                            value = nlohmann::json::parse(value_str);
                        } catch (const nlohmann::json::parse_error& e) {
                            std::cout << "Invalid JSON format: " << value_str << std::endl;
                            valid_value_type = false;
                        }
                    } else {
                        std::cout << "Invalid value type: " << value_type << std::endl;
                        valid_value_type = false;
                    }
                    
                    if (valid_value_type) {
                        entry->set_value(key, value);
                        std::cout << "Added: (" << key << ") to document: (" << docname << ")" << std::endl;
                    }
                } else if (cmd == "remove") {
                    bool removed = entry->remove_value(key);
                    if (removed) {
                        std::cout << "Removed: " << key << std::endl;
                    } else {
                        std::cout << "Could not remove: : " << key << std::endl;
                    }
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
                    auto coll_object = db_object->create_collection(coll_name);

                    // Loop through each collection folder and add JSON files as documents
                    std::string coll_path = db_path + "/" + coll_name;
                    for (const auto& doc_entry : std::filesystem::directory_iterator(coll_path)) {
                        if (doc_entry.is_regular_file()) {
                            std::string doc_name = doc_entry.path().stem().string(); // Get the filename without extension
                            std::string doc_path = coll_path + "/" + doc_entry.path().filename().string();

                            // Read the JSON file and create a document with the data
                            std::ifstream doc_file(doc_path);
                            if (doc_file.is_open()) {
                                nlohmann::json doc_data;
                                doc_file >> doc_data;
                                doc_file.close();

                                // Create the document and set its data
                                auto doc_object = coll_object->create_document(doc_name); // Pass the collection name instead of the document name
                                doc_object->set_data(doc_data);
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    Surface surface;
    TaskQueue task_queue;
    std::string input;
    std::string current_db;
    std::string data_directory = "snugdb_data";

    initialize_data_storage(surface, data_directory);

    task_queue.start();

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
            create_database(surface, task_queue, dbname);
        } else if (command == "gotodb") {
            std::string dbname;
            iss >> dbname;
            go_to_database(surface, dbname, current_db);
        } else if (command == "createcol") {
            std::string colname;
            iss >> colname;
            create_collection(surface, task_queue,colname);
        } else if (command == "dropcol") {
            std::string colname;
            iss >> colname;
            drop_collection(surface,colname);
        } else if (command == "addto") {
                std::string colname, doc_type, docname;
                iss >> colname >> doc_type >> docname;
                add_document(surface, task_queue, colname, doc_type, docname);
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

    task_queue.stop();

    return 0;
}