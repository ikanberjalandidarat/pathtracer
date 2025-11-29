#pragma once
#include <vector>
#include "vec3.h"

inline double l2_diff(const std::vector<vec3>& a,
                      const std::vector<vec3>& b)
{
    double acc = 0.0;
    size_t n = a.size();
    for (size_t i = 0; i < n; ++i) {
        vec3 d = a[i] - b[i];
        acc += d.length_squared();
    }
    return acc / static_cast<double>(n);
}
