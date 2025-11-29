#pragma once
#include "hittable.h"

class sphere : public hittable {
public:
    vec3 center;
    double radius;
    int material_id;

    sphere() {}
    sphere(vec3 cen, double r, int m_id) : center(cen), radius(r), material_id(m_id) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = std::sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || root > t_max) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || root > t_max)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.material_id = material_id;
        return true;
    }
};
