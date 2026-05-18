#!/usr/bin/env bash

g++ -std=c++17 -O2 -static -o regex_matcher src/*.cpp

tar -czf test_bins.tar.gz regex_matcher

rm -f regex_matcher