cmake_minimum_required(VERSION 3.10)
project(snugdb)

find_package(Boost REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directory
include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-deprecated-declarations)
endif()

# Build snugdb-cli executable
add_executable(snugdb-cli snugdb-cli/main.cpp)
target_sources(snugdb-cli PRIVATE snugdb-main/key_value_database.cpp snugdb-main/collection.cpp snugdb-main/document.cpp snugdb-main/surface.cpp snugdb-main/file_utils.cpp snugdb-main/task_queue.cpp)

# Create an executable for the tests
add_executable(test_all snugdb-tests/test_all.cpp)
target_sources(test_all PRIVATE snugdb-main/collection.cpp snugdb-main/document.cpp snugdb-main/surface.cpp snugdb-main/key_value_database.cpp)

add_executable(snugdb-http snugdb-http/main.cpp)
target_sources(snugdb-http PRIVATE snugdb-main/key_value_database.cpp snugdb-main/collection.cpp snugdb-main/document.cpp snugdb-main/surface.cpp snugdb-main/file_utils.cpp snugdb-main/task_queue.cpp)