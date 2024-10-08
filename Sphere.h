#ifndef INC_3DRENDERER_SPHERE_H
#define INC_3DRENDERER_SPHERE_H
#include <utility>
#include "Hittable.h"

class Sphere : public Hittable
{
public:
    Sphere(const Point &center, double radius, std::shared_ptr<Material> mat) : center(center), radius(std::fmax(0, radius)), mat(std::move(mat)) {}

    bool hit(const Ray &r, const Interval &rayT, HitRecord &rec) const override;

private:
    Point center;
    double radius;
    std::shared_ptr<Material> mat;
};
#endif//INC_3DRENDERER_SPHERE_H