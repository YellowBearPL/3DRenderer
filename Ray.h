#ifndef INC_3DRENDERER_RAY_H
#define INC_3DRENDERER_RAY_H
#include "Light.h"
#include "Object.h"
#include "SDL2/SDL.h"
#include "Geometry.h"
#include <memory>
#include <vector>

SDL_Color operator*(const SDL_Color &color, const double &f);

SDL_Color operator*(const double &f, const SDL_Color &color);

SDL_Color operator+(const SDL_Color &v1, const SDL_Color &v2);

class Ray
{
public:
    Point orig;
    Vec3f dir;

    Ray() = default;

    Ray(const Point &origin, const Vec3f &direction) : orig(origin), dir(direction) {}

    [[nodiscard]] Point at(double t) const { return orig + (t * dir); }

    SDL_Color rayColor();
};
#endif//INC_3DRENDERER_RAY_H