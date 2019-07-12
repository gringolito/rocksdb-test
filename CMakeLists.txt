cmake_minimum_required(VERSION 3.14.5)
include("${CMAKE_CURRENT_LIST_DIR}/meta-info.in")
project(rocksdb-test VERSION ${project_version} DESCRIPTION ${project_description})

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})

# ##################################################################################################
# Build project
# ##################################################################################################
add_subdirectory(include)
add_subdirectory(third-party)
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
