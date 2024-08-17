#include "Ray.h"

void Ray::computePrimRay(int x, int y)
{
    auto xx = float(((2 * (x + 0.5) * invWidth) - 1) * angle * aspectratio);
    auto yy = float((1 - (2 * (y + 0.5) * invHeight)) * angle);
    direction = {xx, yy, -1};
    direction.normalize();
    orig = Vec3f(0);
}