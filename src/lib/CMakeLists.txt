include_directories(${RocksDB_INCLUDE_DIRS} ${nlohmann_json_INCLUDE_DIRS})

add_library(mpsync db.cc sync.cc $<TARGET_OBJECTS:utils>)
set_target_properties(mpsync PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(mpsync ${RocksDB_LIBRARIES})

install(TARGETS mpsync DESTINATION lib)