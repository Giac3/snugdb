cmake_minimum_required(VERSION 3.10)
project(snugdb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directory
include_directories(include)

# Build snugdb-cli executable
add_executable(snugdb-cli snugdb-cli/main.cpp)
target_sources(snugdb-cli PRIVATE snugdb-main/key_value_database.cpp snugdb-main/collection.cpp snugdb-main/document.cpp snugdb-main/surface.cpp snugdb-main/file_utils.cpp)

# Build snugdb-tests executable and link it with the KeyValueDatabase implementation
#add_executable(snugdb-tests snugdb-tests/tests.cpp)
#target_sources(snugdb-tests PRIVATE snugdb-main/key_value_database.cpp)