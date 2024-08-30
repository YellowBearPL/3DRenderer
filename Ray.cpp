#include "Ray.h"

#define MAX_RAY_DEPTH 3

std::vector<std::shared_ptr<Object>> Ray::objects{};
SDL_Color Ray::backgroundColor{};
float Ray::bias{};
Point Ray::lightPosition{};
Light Ray::light{};

SDL_Color operator*(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r * f), static_cast<Uint8>(color.g * f), static_cast<Uint8>(color.b * f), color.a};
}

SDL_Color operator+(const SDL_Color &v1, const SDL_Color &v2)
{
    return {static_cast<Uint8>(v1.r + v2.r), static_cast<Uint8>(v1.g + v2.g), static_cast<Uint8>(v1.b + v2.b), static_cast<Uint8>(v1.a + v2.a)};
}

void Ray::computePrimRay(int x, int y)
{
    auto xx = float(((2 * (x + 0.5) * invWidth) - 1) * angle * aspectratio);
    auto yy = float((1 - (2 * (y + 0.5) * invHeight)) * angle);
    direction = {xx, yy, -1};
    direction.normalize();
    origin = Vec3f(0);
}

SDL_Color Ray::trace(int depth)
{
    std::shared_ptr<Object> object = nullptr;
    float minDistance = INFINITY;
    Point pHit;
    Normal nHit;
    for (auto &k : objects)
    {
        if (k->intersect(*this, pHit, nHit))
        {
            float distance = origin.distance(pHit);
            if (distance < minDistance)
            {
                object = k;
                minDistance = distance;
            }
        }
    }

    if (object == nullptr)
    {
        return backgroundColor;
    }

    if (object->isGlass && depth < MAX_RAY_DEPTH)
    {
        Ray reflectionRay, refractionRay;
        SDL_Color reflectionColor, refractionColor;
        float kr, kt;
        reflectionRay = computeReflectionRay(nHit, pHit);
        reflectionColor = reflectionRay.trace(depth + 1);
        refractionRay = computeRefractionRay(object->indexOfRefraction, nHit, pHit);
        refractionColor = refractionRay.trace(depth + 1);
        direction.fresnel(nHit, kr, kt);
        return (reflectionColor * kr) + (refractionColor * (1 - kr));
    }
    else if (!object->isGlass)
    {
        Ray shadowRay;
        shadowRay.origin = pHit + (nHit * bias);
        shadowRay.direction = (lightPosition - pHit).normalize();
        bool isInShadow = false;
        for (auto &k : objects)
        {
            if (k->intersect(shadowRay))
            {
                isInShadow = true;
                break;
            }
        }

        if (!isInShadow)
        {
            return object->color * light.brightness;
        }
    }

    return backgroundColor;
}

Ray Ray::computeReflectionRay(const Normal &normal, const Point &point) const
{
    Vec3f refldir = direction - (normal * 2 * direction.dot(normal));
    refldir.normalize();
    return {point + (normal * bias), refldir};
}

Ray Ray::computeRefractionRay(float index, const Normal &normal, const Point &point) const
{
    float cosi = -normal.dot(direction);
    float k = 1 - (index * index * ((1 - cosi) * cosi));
    Vec3f refrdir = (direction * index) + (normal * ((index * cosi) - std::sqrt(k)));
    refrdir.normalize();
    return {point - (normal * bias), refrdir};
}