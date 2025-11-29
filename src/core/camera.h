#pragma once
#include "ray.h"

class camera {
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect_ratio) {
        double theta = vfov * M_PI / 180.0;
        double h = std::tan(theta/2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;

        vec3 w = unit_vector(lookfrom - lookat);
        vec3 u = unit_vector(cross(vup, w));
        vec3 v = cross(w, u);

        origin = lookfrom;
        horizontal = viewport_width * u;
        vertical   = viewport_height * v;
        lower_left_corner = origin - horizontal/2 - vertical/2 - w;
    }

    ray get_ray(double s, double t) const {
        return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
    }
};
