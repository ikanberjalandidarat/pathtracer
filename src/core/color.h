#pragma once
#include "vec3.h"
#include <iostream>
#include <algorithm>

inline void write_color(std::ostream &out, const vec3& pixel_color) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Gamma 2.0
    r = std::sqrt(r);
    g = std::sqrt(g);
    b = std::sqrt(b);

    int ir = static_cast<int>(256 * std::clamp(r, 0.0, 0.999));
    int ig = static_cast<int>(256 * std::clamp(g, 0.0, 0.999));
    int ib = static_cast<int>(256 * std::clamp(b, 0.0, 0.999));

    out << ir << " " << ig << " " << ib << "\n";
}
