#ifndef INC_3DRENDERER_VEC3_H
#define INC_3DRENDERER_VEC3_H
#include <cmath>

template<typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    explicit Vec3(T xx) : x(xx), y(xx), z(xx) {}

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    [[nodiscard]] float norm() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    Vec3<T> operator*(float f) const { return {x * f, y * f, z * f}; }

    Vec3<T> &operator*=(float f) { *this = *this * f; return *this; }

    Vec3<T> &normalize(T l=1) { *this *= l / norm(); return *this; }

    [[nodiscard]] T dot(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> operator-(const Vec3<T> &v) const { return {x - v.x, y - v.y, z - v.z}; }

    Vec3<T> operator+(const Vec3<T> &v) const { return {x + v.x, y + v.y, z + v.z}; }

    [[nodiscard]] T length2() const { return (x * x) + (y * y) + (z * z); }

    [[nodiscard]] T distance(const Vec3<T> &v) const { return sqrt((v - *this).length2()); }

    void fresnel(const Vec3<float> &direction, float &kr, float &kt) { float facingratio = -direction.dot(*this); kr = float(0.1 + (pow(1 - facingratio, 3) * .9)); kt = 1 - kr; }
};

using Vec3f = Vec3<float>;
using Point = Vec3f;
using Normal = Vec3f;
#endif//INC_3DRENDERER_VEC3_H