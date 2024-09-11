#include "HittableList.h"

bool HittableList::hit(const Ray &r, double rayTmin, double rayTmax, HitRecord &rec) const
{
    HitRecord tempRec;
    bool hitAnything = false;
    double closestSoFar = rayTmax;
    for (const auto &object : objects)
    {
        if (object->hit(r, rayTmin, closestSoFar, tempRec))
        {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}