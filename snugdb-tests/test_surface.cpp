#include "catch.hpp"
#include "../snugdb-main/surface.h"

TEST_CASE("Surface class tests", "[surface]") {
    Surface surface;

    // Test create_database and get_database
    SECTION("create and get database") {
        auto db1 = surface.create_database("test_db1");
        REQUIRE(db1 != nullptr); // Check if the created database is not null

        auto retrieved_db1 = surface.get_database("test_db1");
        REQUIRE(retrieved_db1 == db1); // Check if the retrieved database is the same as the created one
    }

    // Test show_databases
    SECTION("show databases") {
        auto db1 = surface.create_database("test_db1");
        auto db2 = surface.create_database("test_db2");

        std::string show_result = surface.show_databases();
        REQUIRE(show_result.find("test_db1") != std::string::npos); // Check if test_db1 is in the show result
        REQUIRE(show_result.find("test_db2") != std::string::npos); // Check if test_db2 is in the show result
    }

    // Test set_active_database and get_active_database
    SECTION("set and get active database") {
        surface.set_active_database("test_db1");
        REQUIRE(surface.get_active_database() == "test_db1"); // Check if the active database is set correctly

        surface.set_active_database("test_db2");
        REQUIRE(surface.get_active_database() == "test_db2"); // Check if the active database is updated correctly
    }

    // Test get_databases
    SECTION("get all databases") {
        auto db1 = surface.create_database("test_db1");
        auto db2 = surface.create_database("test_db2");

        auto all_databases = surface.get_databases();
        REQUIRE(all_databases.size() == 2); // Check if the correct number of databases is returned
        REQUIRE(all_databases["test_db1"] == db1); // Check if the first database is correct
        REQUIRE(all_databases["test_db2"] == db2); // Check if the second database is correct
    }
}
