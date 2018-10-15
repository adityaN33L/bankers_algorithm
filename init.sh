#!/bin/bash
g++ -std=c++11 bankers.cpp
./a.out <$1 >$2
