find_package(gtest REQUIRED)

add_executable(client-test client.cc $<TARGET_OBJECTS:stubs> $<TARGET_OBJECTS:utils>)
target_link_libraries(client-test mpsync)