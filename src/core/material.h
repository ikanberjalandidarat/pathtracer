#pragma once
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include <cmath>

inline constexpr double PI_MAT = 3.14159265358979323846;

struct Material {
    vec3 albedo;
    vec3 emission;
    bool mirror;

    Material() : albedo(0,0,0), emission(0,0,0), mirror(false) {}
    Material(const vec3& a, const vec3& e, bool m)
        : albedo(a), emission(e), mirror(m) {}
};

inline bool is_emissive(const Material& m) {
    return m.emission.length_squared() > 0.0;
}

// Orthonormal basis for cosine sampling
struct onb {
    vec3 u, v, w;

    void build_from_w(const vec3& n) {
        w = unit_vector(n);
        vec3 a = (std::fabs(w.x()) > 0.9) ? vec3(0,1,0) : vec3(1,0,0);
        v = unit_vector(cross(w, a));
        u = cross(v, w);
    }

    vec3 local(double a, double b, double c) const {
        return a*u + b*v + c*w;
    }
};

// Cosine-weighted hemisphere direction in local coords
inline vec3 random_cosine_direction() {
    double r1 = random_double();
    double r2 = random_double();
    double z = std::sqrt(1.0 - r2);
    double phi = 2.0 * PI_MAT * r1;
    double x = std::cos(phi) * std::sqrt(r2);
    double y = std::sin(phi) * std::sqrt(r2);
    return vec3(x, y, z);
}

// Sample diffuse direction around surface normal (cosine-weighted)
inline vec3 sample_diffuse_direction(const vec3& normal) {
    onb basis;
    basis.build_from_w(normal);
    vec3 d = random_cosine_direction();
    return basis.local(d.x(), d.y(), d.z());
}
