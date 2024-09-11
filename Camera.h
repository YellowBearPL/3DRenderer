#ifndef INC_3DRENDERER_CAMERA_H
#define INC_3DRENDERER_CAMERA_H
#include "Hittable.h"

class Camera
{
public:
    void render(const Hittable &world);

private:
    void initialize();

    SDL_Color rayColor(const Ray &r, const Hittable &world) const;
};
#endif//INC_3DRENDERER_CAMERA_H