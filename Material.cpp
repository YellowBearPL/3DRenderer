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