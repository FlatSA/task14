#!/bin/bash

clang++ -I/usr/include/highs -L/usr/lib/libhighs.so.1.7.2 -std=c++20 -lhighs -O3 -o pipe pipe1.cpp
