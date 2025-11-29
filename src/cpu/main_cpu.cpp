#include <iostream>
#include <fstream>
#include <vector>
#include "core/path_tracer.h"
#include "core/metrics.h"

int main(int argc, char** argv) {
    int max_iterations = 256;
    if (argc > 1) {
        max_iterations = std::atoi(argv[1]);
    }

    PathTracerState state = make_default_state();
    int W = state.cfg.image_width;
    int H = state.cfg.image_height;

    std::vector<vec3> prev_buffer(W * H, vec3(0,0,0));

    for (int it = 1; it <= max_iterations; ++it) {
        path_tracer_iteration(state);

        auto current = normalize_buffer(state);
        double residual = l2_diff(current, prev_buffer);
        std::cout << "Iteration " << it << " residual = " << residual << "\n";

        prev_buffer = current;
    }

    // Dump final image as PPM
    std::ofstream out("output_cpu.ppm");
    out << "P3\n" << W << " " << H << "\n255\n";

    auto final_img = normalize_buffer(state);
    for (int j = H-1; j >= 0; --j) {
        for (int i = 0; i < W; ++i) {
            write_color(out, final_img[j*W + i],
                        state.iterations * state.cfg.spp_per_iteration);
        }
    }

    std::cout << "Wrote output_cpu.ppm\n";
    return 0;
}
