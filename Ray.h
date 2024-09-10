#ifndef INC_3DRENDERER_RAY_H
#define INC_3DRENDERER_RAY_H
#include "Light.h"
#include "Object.h"
#include "SDL2/SDL.h"
#include "Geometry.h"
#include <memory>
#include <vector>

class Ray
{
public:
    Point orig;
    Vec3f dir;

    Ray() = default;

    Ray(const Point &origin, const Vec3f &direction) : orig(origin), dir(direction) {}

    [[nodiscard]] Point at(double t) const { return orig + (t * dir); }

    static SDL_Color rayColor() { return {0, 0, 0, 255}; }
};
#endif//INC_3DRENDERER_RAY_H