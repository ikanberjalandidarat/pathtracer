#pragma once
#include "vec3.h"
#include <iostream>

inline void write_color(std::ostream &out, const vec3& pixel_color, int samples_per_pixel) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    double scale = 1.0 / samples_per_pixel;
    r = std::sqrt(scale * r);
    g = std::sqrt(scale * g);
    b = std::sqrt(scale * b);

    int ir = static_cast<int>(256 * std::clamp(r, 0.0, 0.999));
    int ig = static_cast<int>(256 * std::clamp(g, 0.0, 0.999));
    int ib = static_cast<int>(256 * std::clamp(b, 0.0, 0.999));

    out << ir << " " << ig << " " << ib << "\n";
}
