#!/usr/bin/env bash
set -e
mkdir -p ../build
cd ../build
cmake -DBUILD_MPI=OFF -DBUILD_HIP=OFF -DBUILD_GUI=OFF ..
make -j pathtracer_cpu
./pathtracer_cpu 256