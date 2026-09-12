#!/bin/zsh
CURR_DIR=$pwd
cpp_file=$(find . -name *.cpp)

echo "$cpp_file"


clang++ "$cpp_file" -std=c++17 \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o out

