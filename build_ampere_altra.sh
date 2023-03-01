#!/bin/bash
mkdir build || true
rm -Rf build/*
cd build
export ACL_ROOT_DIR=../arm_compute_library
cmake .. -DDNNL_AARCH64_USE_ACL=ON
make -j8 install
