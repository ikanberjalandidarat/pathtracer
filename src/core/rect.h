#pragma once
#include "hittable.h"

class xy_rect : public hittable {
public:
    double x0, x1, y0, y1, k;
    int material_id;

    xy_rect() {}
    xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, int m)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), material_id(m) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (k - r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max) return false;
        double x = r.origin().x() + t * r.direction().x();
        double y = r.origin().y() + t * r.direction().y();
        if (x < x0 || x > x1 || y < y0 || y > y1) return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(0, 0, 1);
        rec.set_face_normal(r, outward_normal);
        rec.material_id = material_id;
        return true;
    }
};

class xz_rect : public hittable {
public:
    double x0, x1, z0, z1, k;
    int material_id;

    xz_rect() {}
    xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, int m)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), material_id(m) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (k - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max) return false;
        double x = r.origin().x() + t * r.direction().x();
        double z = r.origin().z() + t * r.direction().z();
        if (x < x0 || x > x1 || z < z0 || z > z1) return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(0, 1, 0);
        rec.set_face_normal(r, outward_normal);
        rec.material_id = material_id;
        return true;
    }
};

class yz_rect : public hittable {
public:
    double y0, y1, z0, z1, k;
    int material_id;

    yz_rect() {}
    yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, int m)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), material_id(m) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (k - r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max) return false;
        double y = r.origin().y() + t * r.direction().y();
        double z = r.origin().z() + t * r.direction().z();
        if (y < y0 || y > y1 || z < z0 || z > z1) return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(1, 0, 0);
        rec.set_face_normal(r, outward_normal);
        rec.material_id = material_id;
        return true;
    }
};
