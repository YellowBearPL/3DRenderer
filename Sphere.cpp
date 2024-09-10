#include "Sphere.h"
#include <cmath>
#include "Ray.h"

bool Sphere::intersect(const Ray &ray, Point &point, Normal &normal) const
{
    Vec3f l = center - ray.orig;
    float tca = l.dot(ray.dir);
    if (tca < 0)
    {
        return false;
    }

    float d2 = l.dot(l) - (tca * tca);
    if (d2 > radius2)
    {
        return false;
    }

    float thc = std::sqrt(radius2 - d2);
    float t = tca - thc;
    if (t < 0)
    {
        t = tca + thc;
    }

    point = ray.orig + ray.dir * t;
    normal = point - center;
    normal.normalize();
    return true;
}