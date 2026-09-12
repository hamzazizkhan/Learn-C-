#!/bin/zsh

clang++ boid.cpp -std=c++17 \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o boids

