#ifndef INC_3DRENDERER_GEOMETRY_H
#define INC_3DRENDERER_GEOMETRY_H
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ostream>
#include <vector>
#include "Rt.h"

template<typename T>
class Mat44;

template<typename T>
class Mat33;

template<typename T>
class Mat22;

template<typename T>
class Vec3;

extern const int imageWidth;
extern const int imageHeight;

template<typename T>
class Vec2
{
public:
    T u, v;

    Vec2() : u(0), v(0) {}

    Vec2(T u, T v) : u(u), v(v) {}

    T &operator[](const size_t i) { return i <= 0 ? u : v; }

    const T &operator[](const size_t i) const { return i <= 0 ? u : v; }

    Vec2<T> operator+(const Vec2<T> &v2) const { return {u + v2.u, v + v2.v}; }

    Vec2<T> operator-(const Vec2<T> &v2) const { return {u - v2.u, v - v2.v}; }

    Vec2<T> operator*(float f) const { return Vec2<T>(u * f, v * f); }

    static void line(int x0, int y0, int x1, int y1, SDL_Renderer *image, SDL_Color color);

    void line(const Vec2<T> &p1, SDL_Renderer *image, SDL_Color color) { line(u, v, p1.u, p1.v, image, color); }
};

template<typename T>
class Vec4
{
    std::array<T, 4> data;

public:
    T &operator[](const size_t i) { return data[i]; }

    const T &operator[](const size_t i) const { return data[i]; }

    T operator*(const Vec4<T>& v);

    Vec4<T> operator/(const T &t);

    Vec4<T> &operator/=(const T &t);
};

template<typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    explicit Vec3(T xx) : x(xx), y(xx), z(xx) {}

    explicit Vec3(const Mat44<T> &m) : x(m[0][0] / m[3][0]), y(m[1][0] / m[3][0]), z(m[2][0] / m[3][0]) {}

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    T &operator[](const size_t i) { return i <= 0 ? x : (i <= 1 ? y : z); }

    const T &operator[](const size_t i) const { return i <= 0 ? x : (1 == i ? y : z); }

    T operator*(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> operator+(const Vec3<T> &v) const { return {x + v.x, y + v.y, z + v.z}; }

    Vec3<T> &operator+=(const Vec3<T> &v);

    Vec3<T> operator-(const Vec3<T> &v) const { return {x - v.x, y - v.y, z - v.z}; }

    Vec3<T> operator*(T t) const { return {x * t, y * t, z * t}; }

    Vec3<T> &operator*=(T t);

    Vec3<T> cross(const Vec3<T> &v2) const { return {(y * v2.z) - (z * v2.y), (z * v2.x) - (x * v2.z), (x * v2.y) - (y * v2.x)}; }

    Vec3<T> operator^(const Vec3<T> &v) const { return {(y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x)}; }

    [[nodiscard]] T dot(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    [[nodiscard]] T length2() const { return (x * x) + (y * y) + (z * z); }

    [[nodiscard]] T distance(const Vec3<T> &v) const { return sqrt((v - *this).length2()); }

    Vec3<T> operator/(const T &t) const { return *this * (1 / t); }

    Vec3<T> &operator/=(const T &t);

    Vec3<T> operator-() const { return {-x, -y, -z}; }

    T length() const { return std::sqrt(lengthSquared()); }

    T lengthSquared() const { return (x * x) + (y * y) + (z * z); }

    [[nodiscard]] bool nearZero() const;

    static Vec3<T> random() { return {randomFloat(), randomFloat(), randomFloat()}; }

    static Vec3<T> random(T min, T max) { return {randomFloat(min, max), randomFloat(min, max), randomFloat(min, max)}; }

    Vec3<T> unitVector() const { return *this / length(); }

    static Vec3<T> randomInUnitDisk();

    static Vec3<T> randomUnitVector();

    Vec3<T> reflect(const Vec3<T> &n) const { return *this - (2 * dot(n) * n); }

    Vec3<T> refract(const Vec3<T> &n, float etaiOverEtat);

    T edgeFunction(const Vec3<T> &b, const Vec3<T> &c) { return (c.x - x) * (b.y - y) - (c.y - y) * (b.x - x); }
};

using Vec3f = Vec3<float>;
using Point = Vec3f;
using Normal = Vec3f;
using Vec3i = Vec3<int>;

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vec3<T> &v) { return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2]; }

template<typename T>
Vec3<T> operator*(T t, const Vec3<T> &v) { return v * t; }

template<typename T>
class Dt2
{
public:
    static T det(const Mat22<T> &src);
};

template<typename T>
class Dt3
{
public:
    static T det(const Mat33<T> &src);
};

template<typename T>
class Mat22
{
    std::array<Vec2<T>, 2> rows;

public:
    Vec2<T> &operator[](const size_t idx) { return rows[idx]; }

    const Vec2<T> &operator[](const size_t idx) const { return rows[idx]; }

    T det() const { return Dt2<T>::det(*this); }

    T getMinor(size_t row, size_t col) const;

    [[nodiscard]] T cofactor(size_t row, size_t col) const { return getMinor(row, col) * ((row + col) % 2 ? -1 : 1); }
};

template<typename T>
class Mat33
{
    std::array<Vec3<T>, 3> rows;

public:
    Vec3<T> &operator[](const size_t idx) { return rows[idx]; }

    const Vec3<T> &operator[](const size_t idx) const { return rows[idx]; }

    Vec3<T> col(size_t idx) const;

    T det() const { return Dt3<T>::det(*this); }

    Mat22<T> getMinor(size_t row, size_t col) const;

    [[nodiscard]] T cofactor(size_t row, size_t col) const { return getMinor(row, col).det() * ((row + col) % 2 ? -1 : 1); }

    Mat33<T> adjugate() const;

    Mat33<T> invertTranspose();

    Mat33<T> invert() { return invertTranspose().transpose(); }

    Mat33<T> transpose();

    Vec3<T> operator*(const Vec3<T> &v);

    Mat33<T> operator/(const T &t);
};

template<typename T>
class Mat43
{
    std::array<Vec3<T>, 4> rows;

public:
    Vec3<T> &operator[] (const size_t idx) { return rows[idx]; }

    Vec4<T> col(size_t idx) const;
};

template<typename T>
class Mat44
{
    std::array<Vec4<T>, 4> rows;

public:
    explicit Mat44() = default;

    Vec4<T> &operator[](const size_t idx) { return rows[idx]; }

    const Vec4<T> &operator[](const size_t idx) const { return rows[idx]; }

    [[nodiscard]] Vec4<T> col(size_t idx) const;

    [[nodiscard]] Mat33<T> getMinor(size_t row, size_t col) const;

    [[nodiscard]] T cofactor(size_t row, size_t col) const { return getMinor(row, col).det() * ((row + col) % 2 ? -1 : 1); }

    [[nodiscard]] Mat44<T> adjugate() const;

    Mat44<T> invertTranspose();

    Mat44<T> invert() { return invertTranspose().transpose(); }

    Mat44<T> transpose();

    Vec4<T> operator*(const Vec4<T> &v);

    Mat43<T> operator*(const Mat43<T> &m);

    Mat44<T> operator*(const Mat44<T> &m);

    Mat44<T> operator/(const T &t);
};
#endif//INC_3DRENDERER_GEOMETRY_H