add_subdirectory(utils)
add_subdirectory(lib)

if(MPSYNC_BUILD_STUBS)
    add_subdirectory(stubs)
endif()

add_executable(put-db put.cc)
set_target_properties(put-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(put-db mpsync)

add_executable(get-db get.cc)
set_target_properties(get-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(get-db mpsync)

add_executable(del-db del.cc)
set_target_properties(del-db PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(del-db mpsync)

install(TARGETS put-db
                get-db
                del-db
                DESTINATION
                bin)
