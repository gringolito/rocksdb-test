add_library(utils OBJECT)
target_sources(utils
               PRIVATE
               file.cc
               dir.cc)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(utils PRIVATE -Wall -Wextra -Werror)
target_compile_features(utils PRIVATE cxx_std_11)
