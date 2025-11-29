#include <mpi.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "core/path_tracer.h"
#include "core/metrics.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int max_iterations = 128;
    if (argc > 1) {
        max_iterations = std::atoi(argv[1]);
    }

    PathTracerState state = make_default_state();
    int W = state.cfg.image_width;
    int H = state.cfg.image_height;

    // Simple 1D decomposition over rows
    int rows_per_rank = H / world_size;
    int y_start = world_rank * rows_per_rank;
    int y_end = (world_rank == world_size - 1) ? H : y_start + rows_per_rank;

    std::vector<vec3> local_accum((y_end - y_start) * W, vec3(0,0,0));
    state.accum_buffer.assign(W*H, vec3(0,0,0));

    for (int it = 1; it <= max_iterations; ++it) {
        // Local iteration
        for (int j = y_start; j < y_end; ++j) {
            for (int i = 0; i < W; ++i) {
                vec3 pixel_color(0,0,0);
                for (int s = 0; s < state.cfg.spp_per_iteration; ++s) {
                    double u = (i + random_double()) / (W - 1);
                    double v = (j + random_double()) / (H - 1);
                    ray r = state.cam.get_ray(u, 1.0 - v);
                    pixel_color += ray_color(r, state.scene, state.cfg.max_depth);
                }
                local_accum[(j - y_start)*W + i] += pixel_color;
            }
        }

        // Gather full image across ranks
        std::vector<vec3> global_accum;
        if (world_rank == 0) {
            global_accum.resize(W * H);
        }

        int local_count = (y_end - y_start) * W * 3;
        std::vector<double> local_flat(local_count);
        for (int idx = 0; idx < (int)local_accum.size(); ++idx) {
            local_flat[3*idx+0] = local_accum[idx].x();
            local_flat[3*idx+1] = local_accum[idx].y();
            local_flat[3*idx+2] = local_accum[idx].z();
        }

        std::vector<int> recvcounts(world_size);
        std::vector<int> displs(world_size);
        if (world_rank == 0) {
            int disp = 0;
            for (int r = 0; r < world_size; ++r) {
                int ys = r * rows_per_rank;
                int ye = (r == world_size - 1) ? H : ys + rows_per_rank;
                int cnt = (ye - ys) * W * 3;
                recvcounts[r] = cnt;
                displs[r] = disp;
                disp += cnt;
            }
        }

        std::vector<double> global_flat;
        if (world_rank == 0) {
            global_flat.resize(W * H * 3);
        }

        MPI_Gatherv(local_flat.data(), local_count, MPI_DOUBLE,
                    world_rank == 0 ? global_flat.data() : nullptr,
                    recvcounts.data(), displs.data(), MPI_DOUBLE,
                    MPI_COMM_WORLD);

        if (world_rank == 0) {
            // Unpack
            for (int j = 0; j < H; ++j) {
                for (int i = 0; i < W; ++i) {
                    int idx = j*W + i;
                    state.accum_buffer[idx] = vec3(
                        global_flat[3*idx+0],
                        global_flat[3*idx+1],
                        global_flat[3*idx+2]
                    );
                }
            }

            auto current = normalize_buffer(state);
            static std::vector<vec3> prev = current;
            double residual = l2_diff(current, prev);
            // Global residual is computed on rank 0, but you could Allreduce if needed
            std::cout << "Iteration " << it << " residual = " << residual << "\n";
            prev = current;

            if (it == max_iterations) {
                std::ofstream out("output_mpi_cpu.ppm");
                out << "P3\n" << W << " " << H << "\n255\n";
                for (int j = H-1; j >= 0; --j) {
                    for (int i = 0; i < W; ++i) {
                        write_color(out, current[j*W + i],
                                    state.iterations * state.cfg.spp_per_iteration);
                    }
                }
                std::cout << "Wrote output_mpi_cpu.ppm\n";
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        state.iterations += 1;
    }

    MPI_Finalize();
    return 0;
}
