#pragma once
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include <vector>

struct Scene {
    hittable_list world;
    std::vector<Material> materials;
};

inline Scene make_cornell_scene() {
    Scene s;
    // Materials: simple placeholders
    int white = 0;
    int red   = 1;
    int green = 2;
    int light = 3;

    s.materials = {
        {vec3(0.73, 0.73, 0.73), false}, // white
        {vec3(0.65, 0.05, 0.05), false}, // red
        {vec3(0.12, 0.45, 0.15), false}, // green
        {vec3(4.0, 4.0, 4.0), false}     // light emitter (handled specially)
    };

    // For simplicity, approximate walls with large spheres
    s.world.add(std::make_shared<sphere>(vec3(0, -1000.5, 0), 1000.0, white));
    s.world.add(std::make_shared<sphere>(vec3(0, 1000.5, 0), 1000.0, white));
    s.world.add(std::make_shared<sphere>(vec3(-1000.5, 0, 0), 1000.0, red));
    s.world.add(std::make_shared<sphere>(vec3(1000.5, 0, 0), 1000.0, green));

    // Light sphere near ceiling
    s.world.add(std::make_shared<sphere>(vec3(0, 4.5, 0), 1.0, light));

    // Objects
    s.world.add(std::make_shared<sphere>(vec3(-1.0, 0.5, -1.0), 0.5, white));
    s.world.add(std::make_shared<sphere>(vec3(1.0, 0.5, 0.0), 0.5, white));

    return s;
}
