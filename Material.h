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

class Lambertian : public Material
{
public:
    explicit Lambertian(const Vec3f &albedo) : albedo(albedo) {}

    bool scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const override;

private:
    Vec3f albedo;
};

class Metal : public Material
{
public:
    explicit Metal(const Vec3f &albedo) : albedo(albedo) {}

    bool scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const override;

private:
    Vec3f albedo;
};
#endif//INC_3DRENDERER_MATERIAL_H