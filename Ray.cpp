#include "Ray.h"

SDL_Color operator*(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r * f), static_cast<Uint8>(color.g * f), static_cast<Uint8>(color.b * f), static_cast<Uint8>(color.a * f)};
}

SDL_Color operator*(const double &f, const SDL_Color &color)
{
    return color * f;
}

SDL_Color operator+(const SDL_Color &c1, const SDL_Color &c2)
{
    return {static_cast<Uint8>(c1.r + c2.r), static_cast<Uint8>(c1.g + c2.g), static_cast<Uint8>(c1.b + c2.b), static_cast<Uint8>(c1.a + c2.a)};
}