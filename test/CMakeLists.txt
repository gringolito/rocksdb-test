find_package(gtest REQUIRED)

add_executable(client-test)
target_sources(client-test PRIVATE client.cc)
target_link_libraries(client-test PRIVATE mpsync utils stubs)
target_compile_options(client-test PRIVATE -Wall -Wextra -Werror)

add_executable(server-test)
target_sources(server-test PRIVATE server.cc)
target_link_libraries(server-test PRIVATE mpsync utils stubs)
target_compile_options(server-test PRIVATE -Wall -Wextra -Werror)
