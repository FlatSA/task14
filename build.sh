#!/bin/bash

clang++ -I/usr/local/include/highs/ -L/usr/local/lib/libhighs.so -std=c++20 -o pipe pipe1.cpp
