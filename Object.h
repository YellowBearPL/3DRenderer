#ifndef INC_3DRENDERER_OBJECT_H
#define INC_3DRENDERER_OBJECT_H
#include "Vec3.h"
#include <SDL2/SDL.h>

class Ray;

class Object
{
public:
    SDL_Color color{};
    bool isGlass{};
    float indexOfRefraction{};

    virtual ~Object() = default;
    virtual bool intersect(const Ray &ray, Point &point, Normal &normal) const = 0;
    [[nodiscard]] bool intersect(const Ray &ray) const { Point pTemp; Normal nTemp; return intersect(ray, pTemp, nTemp); }
};
#endif//INC_3DRENDERER_OBJECT_H