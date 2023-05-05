#include "crow_all.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include "../snugdb-main/surface.h"
#include "../snugdb-main/file_utils.h"
#include "../snugdb-main/task.h"
#include "../snugdb-main/task_queue.h"
#include "json.hpp"
#include <fstream>

void start_http_server(Surface& surface, TaskQueue& task_queue, const std::string& host, int port) {
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    app.route_dynamic("/databases/<string>/collections/<string>/documents/<string>")
        .methods("POST"_method)
        ([&surface, &task_queue](const crow::request& req, std::string database_name, std::string collection_name, std::string document_name) {
            std::string json_string = req.body;
            if (json_string.empty()) {
                return crow::response(400, "Invalid JSON");
            }

            auto db = surface.get_database(database_name);
            if (!db) {
                return crow::response(404, "Database not found");
            }

            auto col = db->get_collection(collection_name);
            if (!col) {
                col = db->create_collection(collection_name);
            }

            auto doc = col->get_document(document_name);
            if (!doc) {
                doc = col->create_document(document_name);
            }
            nlohmann::json nlohmann_json = nlohmann::json::parse(json_string);
            doc->set_data(nlohmann_json);

            Task save_document_task(Task::Operation::SaveDocument, database_name, collection_name, document_name, nlohmann_json);
            task_queue.add_task(save_document_task);
            
            crow::json::wvalue response_json;
            response_json["status"] = "success";
            response_json["message"] = "Document created";

            return crow::response(201, response_json);

    });

    app.route_dynamic("/databases/<string>/collections/<string>/documents/<string>")
        .methods("GET"_method)
        ([&surface](const crow::request& req, std::string database_name, std::string collection_name, std::string document_name) {
            crow::json::wvalue response_json;
            auto db = surface.get_database(database_name);
            if (!db) {
                response_json["status"] = "failure";
                response_json["message"] = "Could not find database";

                return crow::response(404, response_json);
            }

            auto col = db->get_collection(collection_name);
            if (!col) {
                response_json["status"] = "failure";
                response_json["message"] = "Could not find collection";

                return crow::response(404, response_json);
            }

            auto doc = col->get_document(document_name);
            if (!doc) {
                response_json["status"] = "failure";
                response_json["message"] = "Could not find document";

                return crow::response(404, response_json);
            }
            
            nlohmann::json nlohmann_data = doc->get_data();
            std::string nlohmann_data_str = nlohmann_data.dump();
            crow::json::rvalue crow_data = crow::json::load(nlohmann_data_str);
            response_json["status"] = "success";
            response_json["data"] = crow_data;

            return crow::response(201, response_json);

    });

    app.route_dynamic("/databases/<string>/ping")
        .methods("GET"_method)
        ([&surface](const crow::request& req, std::string database_name) {
            crow::json::wvalue response_json;
            auto db = surface.get_database(database_name);
            if (!db) {
                response_json["status"] = "failure";
                response_json["message"] = "Could not find database";

                return crow::response(404, response_json);
            }
            
            response_json["status"] = "success";
            response_json["message"] = "connection success";

            return crow::response(201, response_json);

    });

    app.bindaddr(host).port(port).multithreaded().run();
}

// Initialize database from stored files/folders

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

std::string get_snugdb_data_path() {
    std::string home_dir = std::getenv("HOME");

    if (home_dir.empty()) {
        home_dir = std::getenv("USERPROFILE");
    }

    std::filesystem::path snugdb_data_path = home_dir;
    snugdb_data_path /= "snugdb_data";

    return snugdb_data_path.string();
}


int main(int argc, char* argv[]) {
    Surface surface;
    TaskQueue task_queue;
    std::string input;
    std::string current_db;
    std::string data_directory = get_snugdb_data_path();

    std::string host = "127.0.0.1";
    int port = 3030;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--host" && i + 1 < argc) {
            host = std::string(argv[++i]);
            if (host == "localhost") {
                host = "127.0.0.1"; 
            }
        } else if (std::string(argv[i]) == "--port" && i + 1 < argc) {
            port = std::stoi(std::string(argv[++i]));
        } else {
            std::cerr << "Invalid command-line arguments\n";
            return 1;
        }
    }

    initialize_data_storage(surface, data_directory);
    
    task_queue.start();
    start_http_server(surface, task_queue, host, port);
    
    task_queue.stop();
    return 0;
}