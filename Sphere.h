#ifndef INC_3DRENDERER_SPHERE_H
#define INC_3DRENDERER_SPHERE_H
#include "Hittable.h"

class Sphere : public Hittable
{
public:
    Sphere(const Point &center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const override;

private:
    Point center;
    double radius;
};
#endif//INC_3DRENDERER_SPHERE_H