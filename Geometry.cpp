#include "Geometry.h"

extern Matrix modelView;

template<typename T>
void Vec2<T>::line(int x0, int y0, int x1, int y1, SDL_Renderer *image, SDL_Color color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            SDL_RenderDrawPoint(image, y, x);
        }
        else
        {
            SDL_RenderDrawPoint(image, x, y);
        }

        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

template<typename T>
Vec3<T> Vec2<T>::barycentric(const Vec2<T> &b, const Vec2<T> &c, const Vec2<T> &p)
{
    std::array<Vec3f, 2> s;
    s[1].x = c.v - v;
    s[1].y = b.v - v;
    s[1].z = v - p.v;
    s[0].x = c.u - u;
    s[0].y = b.u - u;
    s[0].z = u - p.u;
    Vec3f vU = s[0].cross(s[1]);
    if (std::abs(vU[2]) > 1e-2)
    {
        return Vec3<T>(1.f - ((vU.x + vU.y) / vU.z), vU.y / vU.z, vU.x / vU.z);
    }

    return {-1, 1, 1};
}

template<typename T>
T Vec2<T>::maxElevationAngle(const std::vector<T> &zbuffer, const Vec2<T> &dir)
{
    float maxangle = 0;
    for (int t = 0; t < 1000; t += 1)
    {
        Vec2<T> cur = *this + (dir * t);
        if (cur.u >= float(imageWidth) || cur.v >= float(imageHeight) || cur.u < 0 || cur.v < 0)
        {
            return maxangle;
        }

        float distance = (*this - cur).norm();
        if (distance < 1.f)
        {
            continue;
        }

        float elevation = zbuffer[int(cur.u) + (int(cur.v) * imageWidth)] - zbuffer[int(u) + int(v) * imageWidth];
        maxangle = std::max(maxangle, atanf(elevation / distance));
    }

    return maxangle;
}


template<typename T>
T Vec4<T>::operator*(Vec4<T> const &v)
{
    T ret{};
    for (size_t i = 4; i--;)
    {
        ret += (*this)[i] * v[i];
    }

    return ret;
}

template<typename T>
Vec4<T> Vec4<T>::operator/(T const &t)
{
    for (size_t i = 4; i--;)
    {
        (*this)[i] /= t;
    }

    return *this;
}

template<typename T>
Vec4<T> &Vec4<T>::operator/=(T const &t)
{
    *this = *this / t;
    return *this;
}

template<typename T>
Vec2<T> Vec4<T>::proj2()
{
    Vec2<T> ret;
    ret.u = (*this)[0];
    ret.v = (*this)[1];
    return ret;
}

template<typename T>
Vec3<T> &Vec3<T>::normalize(T l)
{
    *this *= l / norm();
    return *this;
}

template<typename T>
Vec3<T> &Vec3<T>::operator+=(Vec3<T> const &v)
{
    *this = *this + v;
    return *this;
}

template<typename T>
Vec3<T> &Vec3<T>::operator*=(T t)
{
    *this = *this * t;
    return *this;
}

template<typename T>
Vec4<T> Vec3<T>::embed4(T fill)
{
    Vec4<T> ret{};
    ret[3] = fill;
    ret[2] = z;
    ret[1] = y;
    ret[0] = x;
    return ret;
}

template<typename T>
void Vec3<T>::fresnel(Vec3<T> const &direction, T &kr, T &kt)
{
    float facingratio = -direction.dot(*this);
    kr = float(0.1 + (pow(1 - facingratio, 3) * .9));
    kt = 1 - kr;
}

template<typename T>
void Vec3<T>::lookat(const Vec3<T> &center, const Vec3<T> &up)
{
    Vec3<float> vZ = (*this - center).normalize();
    Vec3<float> vX = up.cross(vZ).normalize();
    Vec3<float> vY = vZ.cross(vX).normalize();
    Matrix minv = Matrix::identity();
    Matrix tr = Matrix::identity();
    minv[0][0] = vX.x;
    minv[1][0] = vY.x;
    minv[2][0] = vZ.x;
    tr[0][3] = -x;
    minv[0][1] = vX.y;
    minv[1][1] = vY.y;
    minv[2][1] = vZ.y;
    tr[1][3] = -y;
    minv[0][2] = vX.z;
    minv[1][2] = vY.z;
    minv[2][2] = vZ.z;
    tr[2][3] = -z;
    modelView = minv * tr;
}

template<typename T>
Vec3<T> &Vec3<T>::operator/=(T const &t)
{
    *this = *this / t;
    return *this;
}

template<typename T>
bool Vec3<T>::nearZero() const
{
    T s = 1e-8;
    return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
}

template<typename T>
Vec3<T> Vec3<T>::randomUnitVector()
{
    while (true)
    {
        Vec3<T> p = Vec3<T>::random(-1, 1);
        T lensq = p.lengthSquared();
        if (1e-16 < lensq && lensq <= 1)
        {
            return p / sqrt(lensq);
        }
    }
}

template<typename T>
Vec3<T> Vec3<T>::randomOnHemisphere()
{
    Vec3<T> onUnitSphere = randomUnitVector();
    if (dot(onUnitSphere) > 0.0)
    {
        return onUnitSphere;
    }
    else
    {
        return -onUnitSphere;
    }
}

template<typename T>
T Dt2<T>::det(Mat22<T> const &src)
{
    T ret = 0;
    ret += src[0].v * src.cofactor(0, 1);
    ret += src[0].u * src.cofactor(0, 0);
    return ret;
}

template<typename T>
T Dt3<T>::det(Mat33<T> const &src)
{
    T ret = 0;
    ret += src[0].z * src.cofactor(0, 2);
    ret += src[0].y * src.cofactor(0, 1);
    ret += src[0].x * src.cofactor(0, 0);
    return ret;
}

template<typename T>
T Mat22<T>::getMinor(size_t row, size_t col) const
{
    T ret;
    ret = rows[0 < row ? 0 : 1][0 < col ? 0 : 1];
    return ret;
}

template<typename T>
Vec3<T> Mat33<T>::col(size_t const idx) const
{
    Vec3<T> ret;
    ret.z = rows[2][idx];
    ret.y = rows[1][idx];
    ret.x = rows[0][idx];
    return ret;
}

template<typename T>
Mat22<T> Mat33<T>::getMinor(size_t row, size_t col) const
{
    Mat22<T> ret;
    for (size_t i = 2; i--;)
    {
        ret[i].v = rows[i < row ? i : i + 1][1 < col ? 1 : 2];
        ret[i].u = rows[i < row ? i : i + 1][0 < col ? 0 : 1];
    }

    return ret;
}

template<typename T>
Mat33<T> Mat33<T>::adjugate() const
{
    Mat33<T> ret;
    for (size_t i = 3; i--; )
    {
        ret[i].z = cofactor(i, 2);
        ret[i].y = cofactor(i, 1);
        ret[i].x = cofactor(i, 0);
    }

    return ret;
}

template<typename T>
Mat33<T> Mat33<T>::invertTranspose()
{
    Mat33<T> ret = adjugate();
    T tmp = ret[0] * rows[0];
    return ret / tmp;
}

template<typename T>
Mat33<T> Mat33<T>::transpose()
{
    Mat33<T> ret;
    for (size_t i = 3; i--; ret[i] = col(i));
    return ret;
}

template<typename T>
Vec3<T> Mat33<T>::operator*(Vec3<T> const &v)
{
    Vec3<T> ret;
    ret.z = (*this)[2] * v;
    ret.y = (*this)[1] * v;
    ret.z = (*this)[0] * v;
    return ret;
}

template<typename T>
Mat33<T> Mat33<T>::operator/(T const &t)
{
    for (size_t i = 3; i--; (*this)[i] /= t);
    return *this;
}

template<typename T>
Vec4<T> Mat43<T>::col(size_t const idx) const
{
    Vec4<T> ret{};
    for (size_t i = 4; i--;)
    {
        ret[i] = rows[i][idx];
    }

    return ret;
}

template<typename T>
Mat34<T> Mat43<T>::transpose()
{
    Mat34<T> ret{};
    for (size_t i = 3; i--; ret[i] = col(i));
    return ret;
}

template<typename T>
Vec4<T> Mat44<T>::col(size_t const idx) const
{
    Vec4<T> ret{};
    for (size_t i = 4; i--;)
    {
        ret[i] = rows[i][idx];
    }

    return ret;
}

template<typename T>
Mat44<T> Mat44<T>::identity()
{
    Mat44<T> ret{};
    for (size_t i = 4; i--;)
    {
        for (size_t j = 4; j--;)
        {
            ret[i][j] = (i == j);
        }
    }

    return ret;
}

template<typename T>
Mat33<T> Mat44<T>::getMinor(size_t row, size_t col) const
{
    Mat33<T> ret;
    for (size_t i = 3; i--;)
    {
        ret[i].z = rows[i < row ? i : i + 1][2 < col ? 2 : 3];
        ret[i].y = rows[i < row ? i : i + 1][1 < col ? 1 : 2];
        ret[i].x = rows[i < row ? i : i + 1][0 < col ? 0 : 1];
    }

    return ret;
}

template<typename T>
Mat44<T> Mat44<T>::adjugate() const
{
    Mat44<T> ret{};
    for (size_t i = 4; i--;)
    {
        for (size_t j = 4; j--;)
        {
            ret[i][j] = cofactor(i,j);
        }
    }

    return ret;
}

template<typename T>
Mat44<T> Mat44<T>::invertTranspose()
{
    Mat44<T> ret = adjugate();
    T tmp = ret[0] * rows[0];
    return ret / tmp;
}

template<typename T>
Mat44<T> Mat44<T>::transpose()
{
    Mat44<T> ret{};
    for (size_t i = 4; i--; ret[i] = col(i));
    return ret;
}

template<typename T>
Vec4<T> Mat44<T>::operator*(Vec4<T> const &v)
{
    Vec4<T> ret{};
    for (size_t i = 4; i--;)
    {
        ret[i] = (*this)[i] * v;
    }

    return ret;
}

template<typename T>
Mat43<T> Mat44<T>::operator*(Mat43<T> const &m)
{
    Mat43<T> result;
    for (size_t i = 4; i--;)
    {
        for (size_t j = 3; j--;)
        {
            result[i][j] = (*this)[i] * m.col(j);
        }
    }

    return result;
}

template<typename T>
Mat44<T> Mat44<T>::operator*(Mat44<T> const &m)
{
    Mat44<T> result{};
    for (size_t i = 4; i--;)
    {
        for (size_t j = 4; j--;)
        {
            result[i][j] = (*this)[i] * m.col(j);
        }
    }

    return result;
}

template<typename T>
Mat44<T> Mat44<T>::operator/(T const &t)
{
    for (size_t i = 4; i--; (*this)[i] /= t);
    return *this;
}

template class Vec3<float>;
template class Mat44<float>;
template class Vec4<float>;
template class Vec2<int>;
template class Vec2<float>;
template class Mat33<float>;
template class Mat43<float>;