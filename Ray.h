#ifndef INC_3DRENDERER_RAY_H
#define INC_3DRENDERER_RAY_H
#include "Vec3.h"

extern const float invWidth, invHeight;
extern const float aspectratio;
extern const float angle;

class Ray
{
public:
    Vec3f direction;
    Vec3f orig;

    void computePrimRay(int x, int y);
};
#endif//INC_3DRENDERER_RAY_H