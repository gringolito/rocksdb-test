#!/bin/bash

test -d _build || mkdir _build
cd _build
conan install ..
cmake ..
cmake --build .
cp compile_commands.json ..