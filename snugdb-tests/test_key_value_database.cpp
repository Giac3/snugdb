#include "catch.hpp"
#include "../snugdb-main/key_value_database.h"

TEST_CASE("KeyValueDatabase class tests", "[key_value_database]") {
    KeyValueDatabase db;

    // Test create_collection and get_collection
    SECTION("create and get collection") {
        auto col1 = db.create_collection("test_col1");
        REQUIRE(col1 != nullptr); // Check if the created collection is not null

        auto retrieved_col1 = db.get_collection("test_col1");
        REQUIRE(retrieved_col1 == col1); // Check if the retrieved collection is the same as the created one
    }

    // Test drop_collection
    SECTION("drop collection") {
        auto col1 = db.create_collection("test_col1");
        bool drop_result = db.drop_collection("test_col1");
        REQUIRE(drop_result); // Check if the drop operation was successful

        auto retrieved_col1 = db.get_collection("test_col1");
        REQUIRE(retrieved_col1 == nullptr); // Check if the collection was removed
    }

    // Test show_collections
    SECTION("show collections") {
        auto col1 = db.create_collection("test_col1");
        auto col2 = db.create_collection("test_col2");

        std::string show_result = db.show_collections();
        REQUIRE(show_result.find("test_col1") != std::string::npos); // Check if test_col1 is in the show result
        REQUIRE(show_result.find("test_col2") != std::string::npos); // Check if test_col2 is in the show result
    }

    // Test get_collections
    SECTION("get collections") {
        auto col1 = db.create_collection("test_col1");
        auto col2 = db.create_collection("test_col2");

        auto all_collections = db.get_collections();
        REQUIRE(all_collections.size() == 2); // Check if the correct number of collections is returned
        REQUIRE(all_collections["test_col1"] == col1); // Check if the first collection is correct
        REQUIRE(all_collections["test_col2"] == col2); // Check if the second collection is correct
    }
}
