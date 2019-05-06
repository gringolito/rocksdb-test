include_directories(${RocksDB_INCLUDE_DIRS} ${nlohmann_json_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cc)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${RocksDB_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
