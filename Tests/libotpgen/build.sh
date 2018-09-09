#!/usr/bin/env bash

clang -std=c++1z test.cpp -o libotpgen_test -lstdc++ -lotpgen
