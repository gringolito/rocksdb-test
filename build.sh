#!/bin/bash
bin_name=$(basename "${0}")

function print_help() {
    echo -e "Usage: ${bin_name} [-v] [-h] [-l]"
    echo -e ""
    echo -e "Options:"
    echo -e "  -v        Turn make output verbose"
    echo -e "  -h        Print this help message"
    echo -e "  -l        Use Linux specific stubs"
    exit "${1}"
}

while getopts vhl OPT; do
    case "${OPT}" in
        v) verbose="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON" ;;
        l) stubs='-DMPSYNC_STUBS="linux"' ;;
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
    Darwin) conan_options="-s compiler.libcxx=libc++" ;;
    Linux) conan_options="-s compiler.libcxx=libstdc++11" ;;
    *) ;;
esac

set -e
conan install ${conan_options} --build missing ..
rm -f FindRocksDB.cmake # Remove unwantd Conan generated FindPackage
####################################################################################################
# Build the project
####################################################################################################
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DMPSYNC_BUILD_STUBS=yes ${stubs} -DBUILD_TESTING=yes ${verbose} ..
cmake --build .
cp compile_commands.json ..
