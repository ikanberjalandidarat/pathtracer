# APMA 2822B: Distributed GPU Path Tracer

This project implements a progressive Monte Carlo path tracer with CPU, MPI, and HIP/ROCm backends, together with an SDL2 GUI viewer. It is designed as an alternative final project to the 3D Poisson solver, mapping each Poisson requirement to an equivalent rendering task: solving the rendering equation via Monte Carlo integration, domain decomposition over image tiles, progressive iteration, residual/error metrics, and distributed reductions.

A Cornell Box scene is used as the reference domain, analogous to the unit cube with boundary conditions. Rays are traced iteratively and accumulated until convergence of image variance.

The implementation is intended for AMD GPUs (ROCm, hipcc) and MPI clusters such as Brown's GPU nodes, and provides a starting point for roofline analysis, profiling, and comparison to Unreal Engine 5's Lumen and Nanite pipelines.
