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
    Metal(const Vec3f &albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const override;

private:
    Vec3f albedo;
    float fuzz;
};

class Dielectric : public Material
{
public:
    explicit Dielectric(float refractionIndex) : refractionIndex(refractionIndex) {}

    bool scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const override;

private:
    float refractionIndex;

    static double reflectance(double cosine, double refractionIndex);
};
#endif//INC_3DRENDERER_MATERIAL_H