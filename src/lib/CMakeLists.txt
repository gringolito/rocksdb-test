find_package(RocksDB REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(mpsync)
target_sources(mpsync
               PRIVATE
               db.cc
               sync.cc
               sync_client.cc
               sync_server.cc)
target_link_libraries(mpsync PRIVATE RocksDB::rocksdb utils)
target_include_directories(mpsync
                           PUBLIC ${CMAKE_SOURCE_DIR}/include
                           PRIVATE ${nlohmann_json_INCLUDE_DIRS})
target_compile_options(mpsync PRIVATE -Wall -Wextra -Werror)
target_compile_features(mpsync PUBLIC cxx_std_11)
set_target_properties(mpsync PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

install(TARGETS mpsync DESTINATION lib)
