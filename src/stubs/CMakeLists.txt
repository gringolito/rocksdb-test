if(MPSYNC_STUBS STREQUAL "linux")
    add_subdirectory(linux)
endif()

find_package(fswatch REQUIRED)

add_library(stubs OBJECT)
target_sources(stubs PRIVATE middleware.cc)
target_link_libraries(stubs PRIVATE fswatch::fswatch)
target_include_directories(stubs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(stubs PRIVATE -Wall -Wextra -Werror)
target_compile_features(stubs PUBLIC cxx_std_11)
