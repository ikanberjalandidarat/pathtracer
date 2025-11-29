#pragma once
#include "scene_cornell.h"
#include "camera.h"
#include "color.h"
#include "metrics.h"

inline vec3 ray_color(const ray& r, const Scene& scene, int depth) {
    if (depth <= 0)
        return vec3(0,0,0);

    hit_record rec;
    if (!scene.world.hit(r, 1e-3, 1e9, rec))
        return vec3(0,0,0); // background black

    const Material& mat = scene.materials[rec.material_id];

    // Simple emissive term for light material
    vec3 emitted(0,0,0);
    if (rec.material_id == 3) { // light id
        emitted = mat.albedo;   // high albedo acts as emission
        return emitted;
    }

    ray scattered;
    vec3 attenuation;
    if (!scatter(mat, r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, scene, depth - 1);
}

struct PathTracerConfig {
    int image_width = 400;
    int image_height = 400;
    int max_depth = 10;
    int spp_per_iteration = 1; // rays/px per iteration
};

// struct PathTracerState {
//     Scene scene;
//     camera cam;
//     PathTracerConfig cfg;
//     std::vector<vec3> accum_buffer;  // accumulated radiance
//     int iterations = 0;
// };


struct PathTracerState {
    Scene scene;
    camera cam;
    PathTracerConfig cfg;
    std::vector<vec3> accum_buffer;
    int iterations;

    PathTracerState(const Scene& scene_in,
                    const camera& cam_in,
                    const PathTracerConfig& cfg_in)
        : scene(scene_in),
          cam(cam_in),
          cfg(cfg_in),
          iterations(0)
    {
        accum_buffer.resize(cfg.image_width * cfg.image_height, vec3(0,0,0));
    }
};


// inline PathTracerState make_default_state() {
//     PathTracerState s;

//     s.scene = make_cornell_scene();

//     double aspect = 1.0;
//     vec3 lookfrom(0, 1, 5);
//     vec3 lookat(0, 0.5, 0);
//     vec3 vup(0, 1, 0);

//     s.cfg.image_width = 400;
//     s.cfg.image_height = 400;
//     s.cfg.max_depth = 10;
//     s.cfg.spp_per_iteration = 1;

//     // Explicitly build camera
//     s.cam = camera(
//         lookfrom,
//         lookat,
//         vup,
//         40.0,
//         aspect
//     );

//     s.accum_buffer.resize(
//         s.cfg.image_width * s.cfg.image_height,
//         vec3(0,0,0)
//     );

//     s.iterations = 0;

//     return s;
// }

inline PathTracerState make_default_state() {
    Scene scene = make_cornell_scene();

    double aspect = 1.0;
    vec3 lookfrom(0, 1, 5);
    vec3 lookat(0, 0.5, 0);
    vec3 vup(0, 1, 0);

    PathTracerConfig cfg;
    cfg.image_width = 400;
    cfg.image_height = 400;
    cfg.max_depth = 10;
    cfg.spp_per_iteration = 1;

    camera cam(
        lookfrom,
        lookat,
        vup,
        40.0,
        aspect
    );

    return PathTracerState(scene, cam, cfg);
}



inline void path_tracer_iteration(PathTracerState& state) {
    int W = state.cfg.image_width;
    int H = state.cfg.image_height;
    int spp = state.cfg.spp_per_iteration;

    for (int j = 0; j < H; ++j) {
        for (int i = 0; i < W; ++i) {
            vec3 pixel_color(0,0,0);
            for (int s = 0; s < spp; ++s) {
                double u = (i + random_double()) / (W - 1);
                double v = (j + random_double()) / (H - 1);
                ray r = state.cam.get_ray(u, 1.0 - v);
                pixel_color += ray_color(r, state.scene, state.cfg.max_depth);
            }
            state.accum_buffer[j*W + i] += pixel_color;
        }
    }
    state.iterations += 1;
}

inline std::vector<vec3> normalize_buffer(const PathTracerState& state) {
    int n = state.cfg.image_width * state.cfg.image_height;
    std::vector<vec3> out(n);
    double scale = 1.0 / (state.iterations * state.cfg.spp_per_iteration);
    for (int i = 0; i < n; ++i) {
        out[i] = scale * state.accum_buffer[i];
    }
    return out;
}
