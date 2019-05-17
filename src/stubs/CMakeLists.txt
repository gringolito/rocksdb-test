find_package(fswatch REQUIRED)

add_library(stubs OBJECT middleware.cc)
target_include_directories(stubs PRIVATE ${fswatch_INCLUDE_DIRS})