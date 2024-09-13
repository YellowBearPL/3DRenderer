#ifndef INC_3DRENDERER_CAMERA_H
#define INC_3DRENDERER_CAMERA_H
#include "Hittable.h"

class Camera
{
public:
    static SDL_Renderer *image;
    double aspectRatio = double(imageWidth) / imageHeight;
    int samplesPerPixel = 10;

    void render(const Hittable &world);

private:
    double pixelSamplesScale{};
    Point center;
    Point pixel00Loc;
    Vec3f pixelDeltaU;
    Vec3f pixelDeltaV;

    void initialize();

    [[nodiscard]] Ray getRay(int i, int j) const;

    static Vec3f sampleSquare() { return {static_cast<float>(randomDouble() - 0.5), static_cast<float>(randomDouble() - 0.5), 0}; };

    static Vec3f rayColor(const Ray &r, const Hittable &world);
};
#endif//INC_3DRENDERER_CAMERA_H