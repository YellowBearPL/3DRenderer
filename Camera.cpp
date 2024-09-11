#include "Camera.h"

SDL_Color Camera::rayColor(const Ray &r, const Hittable &world)
{
    HitRecord rec;
    if (world.hit(r, {0, infinity}, rec))
    {
        return {static_cast<Uint8>((rec.normal.x + 1) * 127.5), static_cast<Uint8>((rec.normal.y + 1) * 127.5), static_cast<Uint8>((rec.normal.z + 1) * 127.5), 255};
    }

    Vec3f unitDirection = r.dir.unitVector();
    double a = 0.5 * (unitDirection.y + 1.0);
    return ((1.0 - a) * SDL_Color(255, 255, 255, 255)) + (a * SDL_Color(127, 178, 255, 255));
}