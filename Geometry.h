#ifndef INC_3DRENDERER_GEOMETRY_H
#define INC_3DRENDERER_GEOMETRY_H
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>
#include <array>
#include <cmath>

template<typename T>
class Vec3;

template<typename T>
class Vec2
{
public:
    T u, v;

    Vec2() : u(0), v(0) {}

    Vec2(T u, T v) : u(u), v(v) {}

    Vec2<T> operator+(const Vec2<T> &v2) const { return {u + v2.u, v + v2.v}; }

    Vec2<T> operator-(const Vec2<T> &v2) const { return {u - v2.u, v - v2.v}; }

    Vec2<T> operator*(float f) const { return Vec2<T>(u * f, v * f); }

    Vec2<float> proj2();

    static void line(int x0, int y0, int x1, int y1, SDL_Renderer *image, SDL_Color color);

    void line(const Vec2<T> &p1, SDL_Renderer *image, SDL_Color color) { line(u, v, p1.u, p1.v, image, color); }

    Vec3<T> barycentric(const Vec2<T> &b, const Vec2<T> &c, const Vec2<T> &p);

    T &operator[](const size_t i) { return i <= 0 ? u : v; }

    const T &operator[](const size_t i) const { return i <= 0 ? u : v; }

    T maxElevationAngle(std::vector<T> zbuffer, Vec2<T> dir);

    float norm() { return std::sqrt((u * u) + (v * v)); }
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

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

    Vec2<T> proj2();

    Vec3<T> proj3();
};

using Vec4f = Vec4<float>;

template<typename T>
class Mat33;

template<typename T>
class Dt3
{
public:
    static T det(const Mat33<T> &src);
};

template<typename T>
class Mat22;

template<typename T>
class Dt2
{
public:
    static T det(const Mat22<T> &src);
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

    T det() const { return Dt3<T>::det(*this); }

    Mat22<T> getMinor(size_t row, size_t col) const;

    [[nodiscard]] T cofactor(size_t row, size_t col) const { return getMinor(row, col).det() * ((row + col) % 2 ? -1 : 1); }

    void setCol(size_t idx, Vec3<T> v);

    Vec3<T> operator*(const Vec3<T> &v);

    Vec3<T> col(size_t idx) const;

    Mat33<T> adjugate() const;

    Mat33<T> operator/(const T &t);

    Mat33<T> transpose();

    Mat33<T> invertTranspose();

    Mat33<T> invert() { return invertTranspose().transpose(); }
};

template<typename T>
class Mat34
{
    std::array<Vec4<T>, 3> rows;

public:
    Vec4<T> &operator[](const size_t idx) { return rows[idx]; }
};

template<typename T>
class Mat43
{
    std::array<Vec3<T>, 4> rows;

public:
    void setCol(size_t idx, const Vec4<T> &v) { for (size_t i = 4; i--; rows[i][idx] = v[i]); }

    Vec3<T> &operator[] (const size_t idx) { return rows[idx]; }

    Vec4<T> col(size_t idx) const;

    Mat34<T> transpose();
};

template<typename T>
class Mat44
{
    std::array<Vec4<T>, 4> rows;

public:
    explicit Mat44() = default;

    static Mat44<T> identity();

    Vec4<T> &operator[](const size_t idx) { return rows[idx]; }

    const Vec4<T> &operator[](const size_t idx) const { return rows[idx]; }

    [[nodiscard]] Vec4<T> col(size_t idx) const;

    Vec4<T> operator*(const Vec4<T> &v);

    Mat44<T> operator*(const Mat44<T> &m);

    [[nodiscard]] Mat33<T> getMinor(size_t row, size_t col) const;

    [[nodiscard]] T cofactor(size_t row, size_t col) const { return getMinor(row, col).det() * ((row + col) % 2 ? -1 : 1); }

    [[nodiscard]] Mat44<T> adjugate() const;

    Mat44<T> invertTranspose();

    Mat44<T> operator/(const T &t);

    Mat44<T> invert() { return invertTranspose().transpose(); }

    Mat44<T> transpose();

    Mat43<T> operator*(const Mat43<T> &m);
};

using Matrix = Mat44<float>;

extern const int width;
extern const int height;

template<typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    explicit Vec3(T xx) : x(xx), y(xx), z(xx) {}

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    explicit Vec3(const Matrix &m) : x(m[0][0] / m[3][0]), y(m[1][0] / m[3][0]), z(m[2][0] / m[3][0]) {}

    Vec3<T> operator^(const Vec3<T> &v) const { return {(y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x)}; }

    [[nodiscard]] T norm() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    Vec3<T> operator*(T f) const { return {x * f, y * f, z * f}; }

    T operator*(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> &operator*=(T f);

    Vec3<T> &normalize(T l=1);

    [[nodiscard]] T dot(const Vec3<T> &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

    Vec3<T> operator-(const Vec3<T> &v) const { return {x - v.x, y - v.y, z - v.z}; }

    Vec3<T> operator+(const Vec3<T> &v) const { return {x + v.x, y + v.y, z + v.z}; }

    [[nodiscard]] T length2() const { return (x * x) + (y * y) + (z * z); }

    [[nodiscard]] T distance(const Vec3<T> &v) const { return sqrt((v - *this).length2()); }

    void fresnel(const Vec3<T> &direction, T &kr, T &kt);

    Vec3<T> world2Screen() { return Vec3<T>(((x + 4.) * width / 8.) + .5, ((y + 4.) * height / 8.) + .5, z); }

    Vec3<T> cross(const Vec3<T> &v2) const { return {(y * v2.z) - (z * v2.y), (z * v2.x) - (x * v2.z), (x * v2.y) - (y * v2.x)}; }

    Vec3<T> barycentric(const Vec3<T> &b, const Vec3<T> &c, const Vec3<T> &p);

    void lookat(const Vec3<T> &center, const Vec3<T> &up);

    T &operator[](const size_t i) { return i <= 0 ? x : (i <= 1 ? y : z); }

    const T &operator[](const size_t i) const { return i <= 0 ? x : (1 == i ? y : z); }

    Vec4<T> embed4(T fill = 1);

    Vec3<T> operator/(const T &t);

    Vec3<T> &operator/=(const T &t);
};

using Vec3f = Vec3<float>;
using Point = Vec3f;
using Normal = Vec3f;
using Vec3i = Vec3<int>;

template<typename T>
class Mat23
{
    std::array<Vec3<T>, 2> rows;

public:
    Vec3<T> &operator[](const size_t idx) { return rows[idx]; }

    void setCol(size_t idx, const Vec2<T> &v);

    Vec2<T> operator*(const Vec3<T> &v);
};

template<typename T>
class Mat32
{
    std::array<Vec2<T>, 3> rows;

public:
    Vec2<T> &operator[](const size_t idx) { return rows[idx]; }
};
#endif//INC_3DRENDERER_GEOMETRY_H