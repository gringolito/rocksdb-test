#!/bin/bash

test -d _build || mkdir _build
cd _build
cmake ..
cmake --build .