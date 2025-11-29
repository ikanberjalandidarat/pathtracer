#pragma once
#include "ray.h"
#include "vec3.h"

struct Material {
    vec3 albedo;
    bool is_mirror;
};

inline bool scatter(const Material& mat, const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) {
    if (mat.is_mirror) {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = mat.albedo;
        return true;
    } else {
        vec3 scatter_dir = rec.normal + random_unit_vector();
        if (scatter_dir.length_squared() < 1e-8)
            scatter_dir = rec.normal;
        scattered = ray(rec.p, scatter_dir);
        attenuation = mat.albedo;
        return true;
    }
}
