#ifndef INC_3DRENDERER_SPHERE_H
#define INC_3DRENDERER_SPHERE_H
#include "Object.h"

class Sphere : public Object
{
public:
    Point center;
    float radius, radius2;

    Sphere(const Point &c, const float &r, const SDL_Color &sc, bool glass, float i) : center(c), radius(r), radius2(r * r) { color = sc; isGlass = glass; indexOfRefraction = i; }

    bool intersect(const Ray &ray, Point &point, Normal &normal) const override;
};
#endif//INC_3DRENDERER_SPHERE_H