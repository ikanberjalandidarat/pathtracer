#!/usr/bin/env bash
set -e
mkdir -p ../build
cd ../build
cmake -DBUILD_MPI=ON -DBUILD_HIP=OFF -DBUILD_GUI=OFF ..
make -j pathtracer_mpi_cpu
mpirun -np 8 ./pathtracer_mpi_cpu 128
