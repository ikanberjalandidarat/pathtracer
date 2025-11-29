#pragma once
#include <cmath>
#include <iostream>

class vec3 {
public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0,e1,e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3 &v) {
        e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t; e[1] *= t; e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1.0/t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    static vec3 random(double min = 0.0, double max = 1.0);
};

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.x()+v.x(), u.y()+v.y(), u.z()+v.z());
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.x()-v.x(), u.y()-v.y(), u.z()-v.z());
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.x()*v.x(), u.y()*v.y(), u.z()*v.z());
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.x(), t*v.y(), t*v.z());
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3 &v, double t) {
    return (1.0/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.x()*v.x() + u.y()*v.y() + u.z()*v.z();
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(
        u.y()*v.z() - u.z()*v.y(),
        u.z()*v.x() - u.x()*v.z(),
        u.x()*v.y() - u.y()*v.x()
    );
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

// helpers //

#include <random>

inline double random_double(double min = 0.0, double max = 1.0) {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

inline vec3 vec3::random(double min, double max) {
    return vec3(random_double(min,max),
                random_double(min,max),
                random_double(min,max));
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v,n) * n;
}

// helper stuff ends // 

vec3 random_in_unit_sphere();
vec3 random_unit_vector();
vec3 reflect(const vec3& v, const vec3& n);
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat);

