#include "Geometry.h"

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
Vec3<T> Vec3<T>::randomInUnitDisk()
{
    while (true)
    {
        Vec3<T> p = {randomFloat(-1, 1), randomFloat(-1, 1), 0};
        if (p.lengthSquared() < 1)
        {
            return p;
        }
    }
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
Vec3<T> Vec3<T>::refract(Vec3<T> const &n, float etaiOverEtat)
{
    T cosTheta = std::fmin(-dot(n), 1.0);
    Vec3<T> rOutPerp = etaiOverEtat * (*this + (cosTheta * n));
    Vec3<T> rOutParallel = float(-std::sqrt(std::fabs(1.0 - rOutPerp.lengthSquared()))) * n;
    return rOutPerp + rOutParallel;
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