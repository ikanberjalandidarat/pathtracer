#!/usr/bin/env bash
set -e
mkdir -p ../build
cd ../build
cmake -DBUILD_MPI=OFF -DBUILD_HIP=ON -DBUILD_GUI=OFF ..
make -j pathtracer_hip
./pathtracer_hip 128
