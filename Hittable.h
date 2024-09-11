#ifndef INC_3DRENDERER_HITTABLE_H
#define INC_3DRENDERER_HITTABLE_H
#include "Geometry.h"

class HitRecord
{
public:
    Point p;
    Vec3f normal;
    double t{};
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const = 0;
};
#endif//INC_3DRENDERER_HITTABLE_H