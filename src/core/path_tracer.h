#pragma once
#include "scene_cornell.h"
#include "camera.h"
#include "color.h"
#include "metrics.h"
#include "material.h"

// Estimate direct lighting from the area light using one-sample NEE
inline vec3 sample_direct_light(const Scene& scene,
                                const hit_record& rec,
                                const Material& mat)
{
    const AreaLight& L = scene.light;
    const Material& lm = scene.materials[L.material_id];

    // Sample a point on the light
    double r1 = random_double();
    double r2 = random_double();
    vec3 p_light = L.p0 + r1 * L.u + r2 * L.v;

    vec3 to_light = p_light - rec.p;
    double dist2 = to_light.length_squared();
    double dist = std::sqrt(dist2);
    vec3 wi = to_light / dist;

    double cos_theta = std::max(0.0, dot(rec.normal, wi));
    double cos_theta_light = std::max(0.0, -dot(L.normal, wi));
    if (cos_theta <= 0.0 || cos_theta_light <= 0.0)
        return vec3(0,0,0);

    // Shadow ray
    ray shadow_ray(rec.p + 1e-3 * rec.normal, wi);
    hit_record shadow_rec;
    if (!scene.world.hit(shadow_ray, 1e-3, dist - 1e-3, shadow_rec))
        return vec3(0,0,0);

    if (shadow_rec.material_id != L.material_id)
        return vec3(0,0,0);

    double pdf = dist2 / (L.area * cos_theta_light);
    if (pdf <= 0.0) return vec3(0,0,0);

    vec3 f = mat.albedo / PI_MAT;  // Lambertian BRDF
    return f * lm.emission * (cos_theta / pdf);
}

struct PathTracerConfig {
    int image_width = 400;
    int image_height = 400;
    int max_depth = 10;
    int spp_per_iteration = 1;
};

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

inline PathTracerState make_default_state() {
    Scene scene = make_cornell_scene();

    PathTracerConfig cfg;
    cfg.image_width = 400;
    cfg.image_height = 400;
    cfg.max_depth = 20;
    cfg.spp_per_iteration = 4;

    double aspect = static_cast<double>(cfg.image_width) / cfg.image_height;

    // Classic Cornell camera
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);
    double vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect);

    return PathTracerState(scene, cam, cfg);
}

// Recursive path tracer with NEE + RR + cosine sampling
inline vec3 ray_color(const ray& r, const Scene& scene, int depth) {
    if (depth <= 0)
        return vec3(0,0,0);

    hit_record rec;
    if (!scene.world.hit(r, 1e-3, 1e9, rec))
        return vec3(0,0,0);

    const Material& mat = scene.materials[rec.material_id];
    vec3 emitted = mat.emission;

    // If hit light directly, just return emission (no further bounces)
    if (is_emissive(mat))
        return emitted;

    // Direct lighting from the area light
    vec3 direct = sample_direct_light(scene, rec, mat);

    // Russian roulette
    double rr_prob = 0.9;
    if (depth < 3) rr_prob = 1.0;

    if (random_double() > rr_prob)
        return emitted + direct;

    // Cosine-weighted diffuse bounce
    vec3 new_dir = sample_diffuse_direction(rec.normal);
    ray scattered(rec.p + 1e-3 * rec.normal, new_dir);

    vec3 indirect = ray_color(scattered, scene, depth - 1);
    vec3 f = mat.albedo / PI_MAT;

    // Path throughput update; divide by rr_prob for unbiasedness
    vec3 bounce = f * indirect * (1.0 / rr_prob);

    return emitted + direct + bounce;
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
