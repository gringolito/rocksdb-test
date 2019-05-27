add_subdirectory(utils)
add_subdirectory(lib)

if(MPSYNC_BUILD_STUBS)
    add_subdirectory(stubs)
endif()

add_executable(put-db)
target_sources(put-db PRIVATE put.cc)
target_link_libraries(put-db PRIVATE mpsync)
target_compile_options(put-db PRIVATE -Wall -Wextra -Werror)
set_target_properties(put-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(get-db)
target_sources(get-db PRIVATE get.cc)
target_link_libraries(get-db PRIVATE mpsync)
target_compile_options(get-db PRIVATE -Wall -Wextra -Werror)
set_target_properties(get-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(del-db)
target_sources(del-db PRIVATE del.cc)
target_link_libraries(del-db PRIVATE mpsync)
target_compile_options(del-db PRIVATE -Wall -Wextra -Werror)
set_target_properties(del-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

install(TARGETS put-db
                get-db
                del-db
                DESTINATION
                bin)
