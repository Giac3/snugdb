#include "crow_all.h"
#include <iostream>
#include <sstream>
#include "../snugdb-main/surface.h"

/*
void start_http_server(KeyValueDatabase& db) {
    crow::SimpleApp app;

    app.route_dynamic("/collections/<string>/documents")
        .methods("POST"_method)
        ([&db](const crow::request& req, std::string collection_name) {
            auto json = crow::json::load(req.body);

            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            auto col = db.get_collection(collection_name);
            if (!col) {
                col = db.create_collection(collection_name);
            }

            std::string document_name = json["name"].s();
            auto doc = col->create_document(document_name);
            doc->set_data(json["data"]);

            return crow::response(201, "Document created");
    });

    app.port(3030).multithreaded().run();
}
*/

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
    std::string data_directory = "snugdb_data";
    initialize_data_storage(surface, data_directory);
    
    return 0;
}