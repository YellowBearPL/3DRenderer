#ifndef INC_3DRENDERER_RAY_H
#define INC_3DRENDERER_RAY_H
#include "Geometry.h"
#include "Light.h"
#include "SDL2/SDL.h"
#include <memory>
#include <vector>

SDL_Color operator*(const SDL_Color &color, const double &f);

SDL_Color operator*(const double &f, const SDL_Color &color);

SDL_Color operator+(const SDL_Color &c1, const SDL_Color &c2);

class Hittable;

class Ray
{
public:
    Point orig;
    Vec3f dir;

    Ray() = default;

    Ray(const Point &origin, const Vec3f &direction) : orig(origin), dir(direction) {}

    [[nodiscard]] Point at(double t) const { return orig + (float(t) * dir); }
};
#endif//INC_3DRENDERER_RAY_H