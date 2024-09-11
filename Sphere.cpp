#include "Sphere.h"

bool Sphere::hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const
{
    Vec3f oc = center - r.orig;
    float a = r.dir.lengthSquared();
    float h = r.dir.dot(oc);
    double c = oc.lengthSquared() - (radius * radius);
    double discriminant = (h * h) - (a * c);
    if (discriminant < 0)
    {
        return false;
    }

    double sqrtd = std::sqrt(discriminant);
    double root = (h - sqrtd) / a;
    if (root <= rayTmin || rayTmax <= root)
    {
        root = (h + sqrtd) / a;
        if (root <= rayTmin || rayTmax <= root)
        {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / float(radius);
    return true;
}