# libtipc
add_library(tipc OBJECT)
target_sources(tipc PRIVATE tipcutils/libtipc/libtipc.c)
target_include_directories(tipc PUBLIC tipcutils/include)
