#!/usr/bin/env bash

clang -std=c++1z -I/usr/include/otpgen test.cpp -o libotpgen_test -lstdc++ -lotpgen
