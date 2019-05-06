#!/bin/bash
set -e

test -d _build || mkdir _build
cd _build

####################################################################################################
# Install Conan.io dependencies
####################################################################################################
rocksdb_repo_installed=$(conan remote list | grep 'https://api.bintray.com/conan/koeleck/public-conan')
test -n "${rocksdb_repo_installed}" || conan remote add koeleck https://api.bintray.com/conan/koeleck/public-conan

build_cmd=""
rocksdb_built=$(conan search 'RocksDB' | grep 'Existing package recipes')
test -n "${rocksdb_built}" || build_cmd="--build RocksDB"

conan install .. ${build_cmd}

####################################################################################################
# Build the project
####################################################################################################
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake ..
cmake --build .
cp compile_commands.json ..