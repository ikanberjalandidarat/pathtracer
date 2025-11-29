#include <iostream>
#include <vector>
#include <hip/hip_runtime.h>
#include "core/path_tracer.h"
#include "core/metrics.h"

extern "C" void hip_path_tracer_kernel(vec3* accum,
                                       HipConfig cfg,
                                       Scene scene,
                                       camera cam,
                                       int iteration);

int main(int argc, char** argv) {
    int max_iterations = 128;
    if (argc > 1) max_iterations = std::atoi(argv[1]);

    PathTracerState state = make_default_state();
    int W = state.cfg.image_width;
    int H = state.cfg.image_height;

    size_t n_pixels = W * H;
    size_t buffer_bytes = n_pixels * sizeof(vec3);

    vec3* d_accum = nullptr;
    hipMalloc(&d_accum, buffer_bytes);
    hipMemset(d_accum, 0, buffer_bytes);

    HipConfig cfg;
    cfg.width = W;
    cfg.height = H;
    cfg.spp = state.cfg.spp_per_iteration;
    cfg.max_depth = state.cfg.max_depth;

    dim3 block(16, 16);
    dim3 grid((W + block.x - 1) / block.x,
              (H + block.y - 1) / block.y);

    std::vector<vec3> host_accum(n_pixels, vec3(0,0,0));
    std::vector<vec3> prev_frame(n_pixels, vec3(0,0,0));

    for (int it = 1; it <= max_iterations; ++it) {
        hipLaunchKernelGGL(hip_path_tracer_kernel,
                           grid, block, 0, 0,
                           d_accum, cfg, state.scene, state.cam, it);

        hipDeviceSynchronize();

        hipMemcpy(host_accum.data(), d_accum, buffer_bytes, hipMemcpyDeviceToHost);
        state.accum_buffer = host_accum;
        state.iterations = it;

        auto current = normalize_buffer(state);
        double residual = l2_diff(current, prev_frame);
        std::cout << "Iteration " << it << " residual (HIP) = " << residual << "\n";
        prev_frame = current;
    }

    auto final_img = normalize_buffer(state);

    std::ofstream out("output_hip.ppm");
    out << "P3\n" << W << " " << H << "\n255\n";
    for (int j = H-1; j >= 0; --j) {
        for (int i = 0; i < W; ++i) {
            write_color(out, final_img[j*W + i],
                        state.iterations * state.cfg.spp_per_iteration);
        }
    }
    std::cout << "Wrote output_hip.ppm\n";

    hipFree(d_accum);
    return 0;
}
