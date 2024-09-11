#ifndef INC_3DRENDERER_HITTABLE_H
#define INC_3DRENDERER_HITTABLE_H
#include "Ray.h"

class HitRecord
{
public:
    Point p;
    Vec3f normal;
    double t{};
    bool frontFace{};

    void setFaceNormal(const Ray &r, const Vec3f &outwardNormal) { frontFace = r.dir.dot(outwardNormal) < 0; normal = frontFace ? outwardNormal : -outwardNormal; }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const = 0;
};
#endif//INC_3DRENDERER_HITTABLE_H