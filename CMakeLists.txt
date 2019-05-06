cmake_minimum_required(VERSION 3.8)
project(rocksdb-test CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})

find_package(RocksDB REQUIRED)
find_package(nlohmann_json REQUIRED)

# ##################################################################################################
# Build project
# ##################################################################################################
add_subdirectory(src)
