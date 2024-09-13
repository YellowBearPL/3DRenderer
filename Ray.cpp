#include "Ray.h"
#include "Hittable.h"
#include "Rt.h"

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