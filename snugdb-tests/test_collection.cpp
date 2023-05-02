#include "catch.hpp"
#include "../snugdb-main/collection.h"
#include "json.hpp"

TEST_CASE("Collection class tests", "[collection]") {
    Collection collection;

    // Test create_document and get_document
    SECTION("create and get document") {
        auto doc1 = collection.create_document("test_doc1");
        REQUIRE(doc1 != nullptr); // Check if the created document is not null

        auto retrieved_doc1 = collection.get_document("test_doc1");
        REQUIRE(retrieved_doc1 == doc1); // Check if the retrieved document is the same as the created one
    }

    // Test drop_document
    SECTION("drop document") {
        auto doc1 = collection.create_document("test_doc1");
        bool drop_result = collection.drop_document("test_doc1");
        REQUIRE(drop_result); // Check if the drop operation was successful

        auto retrieved_doc1 = collection.get_document("test_doc1");
        REQUIRE(retrieved_doc1 == nullptr); // Check if the document was removed
    }

    // Test show_documents
    SECTION("show document") {
    auto doc1 = collection.create_document("test_doc1");

    // Parse the JSON string and set the data in the document
    nlohmann::json json_data = R"({"key": "value"})"_json;
    doc1->set_data(json_data);

    std::string show_result = collection.show_document("test_doc1");

    
    //std::cout << "Show result: " << std::endl << show_result << std::endl;

    REQUIRE(show_result.find("key: \"value\"") != std::string::npos); // Check if the key-value pair is in the show result
}
}