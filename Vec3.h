#ifndef INC_3DRENDERER_VEC3_H
#define INC_3DRENDERER_VEC3_H
#include "Matrix.h"
#include <array>
#include <cmath>

extern const int width;
extern const int height;
extern Matrix modelView;

template<typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    explicit Vec3(T xx) : x(xx), y(xx), z(xx) {}

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec3<T> operator^(const Vec3<T> &v) const { return {(y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x)}; }

    [[nodiscard]] float norm() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    Vec3<T> operator*(float f) const { return {x * f, y * f, z * f}; }

    T operator*(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> &operator*=(float f) { *this = *this * f; return *this; }

    Vec3<T> &normalize(T l=1) { *this *= l / norm(); return *this; }

    [[nodiscard]] T dot(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> operator-(const Vec3<T> &v) const { return {x - v.x, y - v.y, z - v.z}; }

    Vec3<T> operator+(const Vec3<T> &v) const { return {x + v.x, y + v.y, z + v.z}; }

    [[nodiscard]] T length2() const { return (x * x) + (y * y) + (z * z); }

    [[nodiscard]] T distance(const Vec3<T> &v) const { return sqrt((v - *this).length2()); }

    void fresnel(const Vec3<float> &direction, float &kr, float &kt) { float facingratio = -direction.dot(*this); kr = float(0.1 + (pow(1 - facingratio, 3) * .9)); kt = 1 - kr; }

    Vec3<float> world2screen() { return Vec3<float>(int(((x + 4.) * width / 8.) + .5), int(((y + 4.) * height / 8.) + .5), z); }

    Vec3<T> cross(Vec3<T> v2) { return {(y * v2.z) - (z * v2.y), (z * v2.x) - (x * v2.z), (x * v2.y) - (y * v2.x)}; }

    Vec3<float> barycentric(Vec3<float> b, Vec3<float> c, Vec3<float> p) { std::array<Vec3<float>, 2> s; s[1].x = c.y - y; s[1].y = b.y - y; s[1].z = y - p.y; s[0].x = c.x - x; s[0].y = b.x - x; s[0].z = x - p.x; Vec3<float> u = s[0].cross(s[1]); if (std::abs(u.z) > 1e-2) { return {1.f - ((u.x + u.y) / u.z), u.y / u.z, u.x / u.z}; } return {-1, 1, 1}; }

    void lookat(Vec3<float> center, Vec3<float> up) { Vec3<float> vZ = (*this - center).normalize(); Vec3<float> vX = up.cross(z).normalize(); Vec3<float> vY = z.cross(x).normalize(); Matrix minv = Matrix::identity(); Matrix tr = Matrix::identity(); minv[0][0] = vX.x; minv[1][0] = vY.x; minv[2][0] = vZ.x; tr[0][3] = -x; minv[0][1] = vX.y; minv[1][1] = vY.y; minv[2][1] = vZ.y; tr[1][3] = -y; minv[0][2] = vX.z; minv[1][2] = vY.z; minv[2][2] = vZ.z; tr[2][3] = -z;  modelView = minv * tr; }
};

using Vec3f = Vec3<float>;
using Point = Vec3f;
using Normal = Vec3f;
#endif//INC_3DRENDERER_VEC3_H