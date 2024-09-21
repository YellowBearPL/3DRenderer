#include "Sphere.h"

bool Sphere::hit(const Ray &r, const Interval &rayT, HitRecord &rec) const
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
    if (!rayT.surrounds(root))
    {
        root = (h + sqrtd) / a;
        if (!rayT.surrounds(root))
        {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3f outwardNormal = (rec.p - center) / float(radius);
    rec.setFaceNormal(r, outwardNormal);
    rec.mat = mat;
    return true;
}