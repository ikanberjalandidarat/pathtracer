#include <hip/hip_runtime.h>
#include "core/vec3.h"
#include "core/ray.h"
#include "core/path_tracer.h"

extern "C" {

struct HipConfig {
    int width;
    int height;
    int spp;
    int max_depth;
};

__device__ vec3 ray_color_device(const ray& r, const Scene& scene, int depth) {
    // For now, very simple: just background
    if (depth <= 0) return vec3(0,0,0);
    return vec3(0.1, 0.1, 0.1);
}

__global__ void hip_path_tracer_kernel(vec3* accum,
                                       HipConfig cfg,
                                       Scene scene,
                                       camera cam,
                                       int iteration)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= cfg.width || y >= cfg.height) return;

    int idx = y * cfg.width + x;
    vec3 pixel_color(0,0,0);

    for (int s = 0; s < cfg.spp; ++s) {
        double u = (x + 0.5) / (cfg.width - 1);
        double v = (y + 0.5) / (cfg.height - 1);
        ray r = cam.get_ray(u, 1.0 - v);
        pixel_color += vec3(0.2, 0.2, 0.4); // placeholder
    }

    accum[idx] += pixel_color;
}

} // extern "C"
