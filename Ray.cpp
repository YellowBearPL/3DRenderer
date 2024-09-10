#include "Ray.h"

SDL_Color operator*(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r * f), static_cast<Uint8>(color.g * f), static_cast<Uint8>(color.b * f), static_cast<Uint8>(color.a * f)};
}

SDL_Color operator*(const double &f, const SDL_Color &color)
{
    return color * f;
}

SDL_Color operator+(const SDL_Color &v1, const SDL_Color &v2)
{
    return {static_cast<Uint8>(v1.r + v2.r), static_cast<Uint8>(v1.g + v2.g), static_cast<Uint8>(v1.b + v2.b), static_cast<Uint8>(v1.a + v2.a)};
}

SDL_Color Ray::rayColor()
{
    if (Point(0,0,-1).hitSphere(0.5, *this))
    {
        return {255, 0, 0, 255};
    }

    Vec3f unitDirection = dir.unitVector();
    double a = 0.5 * (unitDirection.y + 1.0);
    return ((1.0 - a) * SDL_Color(255, 255, 255, 255)) + (a * SDL_Color(127, 178, 255, 255));
}