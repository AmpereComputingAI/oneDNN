#!/bin/bash

# Before building this please build ACL
# 1. Clone https://github.com/AmpereComputingAI/arm_compute_library to the same level as oneDNN
# 2. cd arm_compute_library
# 3. ./build_ampere_altra.sh

mkdir build || true
rm -Rf build/*
cd build
export ACL_ROOT_DIR=../arm_compute_library
cmake .. -DDNNL_AARCH64_USE_ACL=ON
make -j8 install
