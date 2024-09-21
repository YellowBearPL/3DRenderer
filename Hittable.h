#ifndef INC_3DRENDERER_HITTABLE_H
#define INC_3DRENDERER_HITTABLE_H
#include "Interval.h"
#include "Ray.h"

class Material;

class HitRecord
{
public:
    Point p;
    Vec3f normal;
    std::shared_ptr<Material> mat;
    double t{};
    bool frontFace{};

    void setFaceNormal(const Ray &r, const Vec3f &outwardNormal) { frontFace = r.dir.dot(outwardNormal) < 0; normal = frontFace ? outwardNormal : -outwardNormal; }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &r, const Interval &rayT, HitRecord& rec) const = 0;
};
#endif//INC_3DRENDERER_HITTABLE_H