find_package(RocksDB)
find_package(nlohmann_json)

message ("RocksDB FOUND=${RocksDB_FOUND} INCLUDE_DIR=${RocksDB_INCLUDE_DIR} LIBRARIES=${RocksDB_LIBRARIES}")
message ("json FOUND=${nlohmann_json_FOUND} INCLUDE_DIR=${nlohmann_json_INCLUDE_DIR} LIBRARIES=${nlohmann_json_LIBRARIES}")
include_directories(${RocksDB_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIR} /usr/local/include)
if("${RocksDB_LIBRARIES}" STREQUAL "")
    set(RocksDB_LIBRARIES /usr/local/lib/librocksdb.a)
endif()



add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} ${RocksDB_LIBRARIES})
add_dependencies(${PROJECT_NAME} rocksdb json)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)