find_package(gtest REQUIRED)

add_executable(client-test client.cc $<TARGET_OBJECTS:stubs>)
target_link_libraries(client-test mpsync)