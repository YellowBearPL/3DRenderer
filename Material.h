#ifndef INC_3DRENDERER_MATERIAL_H
#define INC_3DRENDERER_MATERIAL_H
#include "Hittable.h"
#include "Ray.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const { return false; }
};
#endif//INC_3DRENDERER_MATERIAL_H