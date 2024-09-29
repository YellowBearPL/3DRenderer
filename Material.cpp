#include "Material.h"

bool Lambertian::scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const
{
    Vec3f scatterDirection = rec.normal + Vec3f::randomUnitVector();
    if (scatterDirection.nearZero())
    {
        scatterDirection = rec.normal;
    }

    scattered = {rec.p, scatterDirection};
    attenuation = albedo;
    return true;
}

bool Metal::scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const
{
    Vec3f reflected = rIn.dir.reflect(rec.normal);
    reflected = reflected.unitVector() + (fuzz * Vec3f::randomUnitVector());
    scattered = {rec.p, reflected};
    attenuation = albedo;
    return (scattered.dir.dot(rec.normal) > 0);
}

bool Dielectric::scatter(const Ray &rIn, const HitRecord &rec, Vec3f &attenuation, Ray &scattered) const
{
    attenuation = {1.0, 1.0, 1.0};
    float ri = rec.frontFace ? (1.0f / refractionIndex) : refractionIndex;
    Vec3f unitDirection = rIn.dir.unitVector();
    double cosTheta = std::fmin(-unitDirection.dot(rec.normal), 1.0);
    double sinTheta = std::sqrt(1.0 - (cosTheta * cosTheta));
    Vec3f refracted;
    if (ri * sinTheta > 1.0)
    {
        refracted = unitDirection.reflect(rec.normal);
    }
    else
    {
        refracted = unitDirection.refract(rec.normal, ri);
    }

    scattered = {rec.p, refracted};
    return true;
}