#ifndef INC_3DRENDERER_HITTABLE_H
#define INC_3DRENDERER_HITTABLE_H
#include "Ray.h"

class HitRecord
{
public:
    Point p;
    Vec3f normal;
    double t{};

    void setFaceNormal(const Ray &r, const Vec3f &outwardNormal) { Vec3f rayDirection = r.dir; bool frontFace; if (rayDirection.dot(outwardNormal) > 0.0) { normal = -outwardNormal; frontFace = false; } else { normal = outwardNormal; frontFace = true; }}
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const = 0;
};
#endif//INC_3DRENDERER_HITTABLE_H