cmake_minimum_required(VERSION 3.8)
project(rocksdb-test CXX)

set (CMAKE_EXPORT_COMPILE_COMMANDS true)
set (CMAKE_CXX_STANDARD 11)

find_package(RocksDB)
find_package(nlohmann_json)

####################################################################################################
## Build project
####################################################################################################
add_subdirectory(src)