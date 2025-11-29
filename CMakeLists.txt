cmake_minimum_required(VERSION 3.18)
project(APMA2822B_PathTracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_MPI "Build MPI examples" ON)
option(BUILD_HIP "Build HIP GPU examples" ON)
option(BUILD_GUI "Build SDL2 GUI viewer" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Core library (header-only, but we make an interface target)
add_library(pathtracer_core INTERFACE)
target_include_directories(pathtracer_core INTERFACE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# CPU serial executable
add_executable(pathtracer_cpu src/cpu/main_cpu.cpp)
target_link_libraries(pathtracer_cpu PRIVATE pathtracer_core)

# MPI target
if(BUILD_MPI)
    find_package(MPI REQUIRED)
    add_executable(pathtracer_mpi_cpu src/mpi/main_mpi_cpu.cpp)
    target_link_libraries(pathtracer_mpi_cpu PRIVATE pathtracer_core MPI::MPI_CXX)
endif()

# HIP GPU target
if(BUILD_HIP)
    find_package(HIP REQUIRED)
    hip_add_executable(pathtracer_hip
        src/gpu/main_hip.cpp
        src/gpu/hip_kernels.hip.cpp
    )
    target_link_libraries(pathtracer_hip PRIVATE pathtracer_core)
endif()

# GUI target (SDL2)
if(BUILD_GUI)
    find_package(SDL2 REQUIRED)
    add_executable(pathtracer_gui src/gui/main_gui_sdl.cpp)
    target_link_libraries(pathtracer_gui PRIVATE pathtracer_core SDL2::SDL2)
endif()
