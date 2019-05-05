cmake_minimum_required(VERSION 3.8)
project(rocksdb-test CXX)

set (CMAKE_EXPORT_COMPILE_COMMANDS true)
set (CMAKE_CXX_STANDARD 11)

####################################################################################################
## External projects configuration
####################################################################################################
include(ExternalProject)
# set_directory_properties(PROPERTIES
    # EP_BASE ${CMAKE_CURRENT_BINARY_DIR}
# )

ExternalProject_Add(rocksdb
    GIT_REPOSITORY https://github.com/facebook/rocksdb.git
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rocksdb
    UPDATE_DISCONNECTED 1
    # CONFIGURE_COMMAND ""
    # BUILD_COMMAND ""
    # INSTALL_COMMAND ""
)

ExternalProject_Add(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/json
    UPDATE_DISCONNECTED 1
    # CONFIGURE_COMMAND ""
    # BUILD_COMMAND ""
    # INSTALL_COMMAND ""
)

####################################################################################################
## Build project
####################################################################################################
add_subdirectory(src)