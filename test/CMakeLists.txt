find_package(gtest REQUIRED)

add_executable(client)
target_sources(client PRIVATE client.cc)
target_link_libraries(client PRIVATE mpsync stubs tipc)
target_compile_options(client PRIVATE -Wall -Wextra -Werror)

add_executable(server)
target_sources(server PRIVATE server.cc)
target_link_libraries(server PRIVATE mpsync stubs tipc)
target_compile_options(server PRIVATE -Wall -Wextra -Werror)

add_executable(middleware-test)
target_sources(middleware-test PRIVATE middleware.cc)
target_link_libraries(middleware-test PRIVATE gtest::gtest stubs tipc utils)
target_compile_options(middleware-test PRIVATE -Wall -Wextra -Werror)