#!/bin/bash

test -d _build || mkdir _build
cd _build

####################################################################################################
# Install Conan.io dependencies
####################################################################################################
rocksdb_repo_installed=$(conan remote list | grep 'https://api.bintray.com/conan/koeleck/public-conan')
test -n "${rocksdb_repo_installed}" || conan remote add koeleck https://api.bintray.com/conan/koeleck/public-conan

set -e
conan install -s compiler.libcxx=libstdc++11 --build missing ..

####################################################################################################
# Build the project
####################################################################################################
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake ..
cmake --build .
cp compile_commands.json ..