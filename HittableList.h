#ifndef INC_3DRENDERER_HITTABLELIST_H
#define INC_3DRENDERER_HITTABLELIST_H
#include "Hittable.h"

class HittableList : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() = default;

    explicit HittableList(const std::shared_ptr<Hittable>& object) { add(object); }

    void clear() { objects.clear(); }

    void add(const std::shared_ptr<Hittable>& object) { objects.push_back(object); }

    bool hit(const Ray &r, const Interval &rayT, HitRecord &rec) const override;
};
#endif//INC_3DRENDERER_HITTABLELIST_H