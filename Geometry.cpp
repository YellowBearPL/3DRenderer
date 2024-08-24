#include "Geometry.h"

extern Matrix modelView;

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
Vec2<T> Vec4<T>::proj2()
{
    Vec2<T> ret;
    ret.u = (*this)[0];
    ret.v = (*this)[1];
    return ret;
}

template<typename T>
Vec2<float> Vec2<T>::proj2()
{
    Vec2<float> ret;
    ret.u = u;
    ret.v = v;
    return ret;
}

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
Vec3<T> Vec2<T>::barycentric(Vec2<T> b, Vec2<T> c, Vec2<T> p)
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
Vec3<T> &Vec3<T>::operator*=(T f)
{
    *this = *this * f;
    return *this;
}

template<typename T>
Vec3<T> &Vec3<T>::normalize(T l)
{
    *this *= l / norm();
    return *this;
}

template<typename T>
void Vec3<T>::fresnel(Vec3<T> const &direction, T &kr, T &kt)
{
    float facingratio = -direction.dot(*this);
    kr = float(0.1 + (pow(1 - facingratio, 3) * .9));
    kt = 1 - kr;
}

template<typename T>
Vec3<T> Vec3<T>::barycentric(Vec3<T> b, Vec3<T> c, Vec3<T> p)
{
    std::array<Vec3<float>, 2> s;
    s[1].x = c.y - y;
    s[1].y = b.y - y;
    s[1].z = y - p.y;
    s[0].x = c.x - x;
    s[0].y = b.x - x;
    s[0].z = x - p.x;
    Vec3<float> u = s[0].cross(s[1]);
    if (std::abs(u.z) > 1e-2)
    {
        return {1.f - ((u.x + u.y) / u.z), u.y / u.z, u.x / u.z};
    }

    return {-1, 1, 1};
}

template<typename T>
void Vec3<T>::lookat(Vec3<T> center, Vec3<T> up)
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

template class Vec3<float>;
template class Mat44<float>;
template class Vec4<float>;
template class Vec2<int>;
template class Vec2<float>;