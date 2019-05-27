#!/bin/bash
bin_name=$(basename "${0}")

function print_help() {
    echo -e "Usage: ${bin_name} [-v] [-h] [-d]"
    echo -e ""
    echo -e "Options:"
    echo -e "  -v        Turn make output verbose"
    echo -e "  -h        Print this help message"
    echo -e "  -d        Build project in debug mode"
    exit "${1}"
}

while getopts vhd OPT; do
    case "${OPT}" in
        v) verbose="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON" ;;
        d) debug="-DCMAKE_BUILD_TYPE=Debug" ;;
        h) print_help 0 ;;
        *) print_help 1 ;;
    esac
done

test -d _build || mkdir _build
cd _build

####################################################################################################
# Install Conan.io dependencies
####################################################################################################
rocksdb_repo_installed=$(conan remote list | grep 'https://api.bintray.com/conan/koeleck/public-conan')
test -n "${rocksdb_repo_installed}" || conan remote add koeleck https://api.bintray.com/conan/koeleck/public-conan

fswatch_repo_installed=$(conan remote list | grep 'https://api.bintray.com/conan/bincrafters/public-conan')
test -n "${fswatch_repo_installed}" || conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan

os=$(uname -s)
case "${os}" in
    Darwin)
        conan_options="-s compiler.libcxx=libc++"
        stubs="-DMPSYNC_STUBS=freebsd"
        ;;
    Linux)
        conan_options="-s compiler.libcxx=libstdc++11"
        stubs="-DMPSYNC_STUBS=linux"
        ;;
    *) ;;
esac

set -e
conan install ${conan_options} --build missing ..
rm -f FindRocksDB.cmake # Remove unwantd Conan generated FindPackage
####################################################################################################
# Build the project
####################################################################################################
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake ${debug} -DMPSYNC_BUILD_STUBS=yes ${stubs} -DBUILD_TESTING=yes ${verbose} ..
cmake --build .
cp compile_commands.json ..
