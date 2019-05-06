include_directories(${RocksDB_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIR} /usr/local/include)

if("${RocksDB_LIBRARIES}" STREQUAL "")
    set(RocksDB_LIBRARIES /usr/local/lib/librocksdb.a)
endif()

add_executable(${PROJECT_NAME} main.cc)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${RocksDB_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)