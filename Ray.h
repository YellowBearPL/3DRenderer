#ifndef INC_3DRENDERER_RAY_H
#define INC_3DRENDERER_RAY_H
#include "Light.h"
#include "Object.h"
#include "SDL2/SDL.h"
#include "Geometry.h"
#include <memory>
#include <vector>

extern const float invWidth, invHeight;
extern const float aspectratio;
extern const float angle;

SDL_Color operator*(const SDL_Color &color, const double &f);

class Ray
{
public:
    Vec3f direction;
    Vec3f origin;
    static std::vector<std::shared_ptr<Object>> objects;
    static SDL_Color backgroundColor;
    static float bias;
    static Point lightPosition;
    static Light light;

    void computePrimRay(int x, int y);

    SDL_Color trace(int depth);

    [[nodiscard]] Ray computeReflectionRay(const Normal &normal, const Point &point) const;

    [[nodiscard]] Ray computeRefractionRay(float index, const Normal &normal, const Point &point) const;
};
#endif//INC_3DRENDERER_RAY_H