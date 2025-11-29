#pragma once
#include "hittable_list.h"
#include "sphere.h"
#include "rect.h"
#include "material.h"
#include <vector>

struct AreaLight {
    vec3 p0;
    vec3 u;
    vec3 v;
    vec3 normal;
    double area;
    int material_id;
};

struct Scene {
    hittable_list world;
    std::vector<Material> materials;
    AreaLight light;
};

inline Scene make_cornell_scene() {
    Scene s;

    // Indices into materials
    int white = 0;
    int red   = 1;
    int green = 2;
    int light = 3;

    // Materials: Cornell style
    s.materials = {
        Material(vec3(0.73, 0.73, 0.73), vec3(0,0,0), false),  // white
        Material(vec3(0.65, 0.05, 0.05), vec3(0,0,0), false),  // red
        Material(vec3(0.12, 0.45, 0.15), vec3(0,0,0), false),  // green
        Material(vec3(0.0, 0.0, 0.0),   vec3(15,15,15), false) // bright area light
    };

    // Cornell box dimensions from "Ray Tracing: The Next Week" style
    // Box from (0,0,0) to (555,555,555)

    // Left wall (green)  x = 0
    s.world.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));

    // Right wall (red) x = 555
    s.world.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));

    // Floor (white) y = 0
    s.world.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));

    // Ceiling (white) y = 555
    s.world.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white));

    // Back wall (white) z = 555
    s.world.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    // Area light on the ceiling: a rectangle in xz-plane at y = 554
    double lx0 = 213, lx1 = 343;
    double lz0 = 227, lz1 = 332;
    double ly  = 554;

    auto light_rect = std::make_shared<xz_rect>(lx0, lx1, lz0, lz1, ly, light);
    s.world.add(light_rect);

    // Fill area-light descriptor for sampling
    s.light.p0 = vec3(lx0, ly, lz0);
    s.light.u  = vec3(lx1 - lx0, 0, 0);
    s.light.v  = vec3(0, 0, lz1 - lz0);
    s.light.normal = vec3(0, -1, 0); // points down into the box
    s.light.area   = cross(s.light.u, s.light.v).length();
    s.light.material_id = light;

    // Two spheres in the box (white diffuse)
    s.world.add(std::make_shared<sphere>(vec3(185, 82.5, 169), 82.5, white));
    s.world.add(std::make_shared<sphere>(vec3(368, 82.5, 351), 82.5, white));

    return s;
}
