#ifndef INC_3DRENDERER_CAMERA_H
#define INC_3DRENDERER_CAMERA_H
#include "Hittable.h"

class Camera
{
public:
    static SDL_Renderer *image;
    double aspectRatio = double(imageWidth) / imageHeight;

    void render(const Hittable &world);

private:
    Point center;
    Point pixel00Loc;
    Vec3f pixelDeltaU;
    Vec3f pixelDeltaV;

    void initialize();

    static SDL_Color rayColor(const Ray &r, const Hittable &world);
};
#endif//INC_3DRENDERER_CAMERA_H