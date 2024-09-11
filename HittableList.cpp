#include "HittableList.h"

bool HittableList::hit(const Ray &r, const Interval &rayT, HitRecord &rec) const
{
    HitRecord tempRec;
    bool hitAnything = false;
    double closestSoFar = rayT.max;
    for (const auto &object : objects)
    {
        if (object->hit(r, {rayT.min, closestSoFar}, tempRec))
        {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}