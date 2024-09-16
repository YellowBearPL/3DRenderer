#ifndef INC_3DRENDERER_CAMERA_H
#define INC_3DRENDERER_CAMERA_H
#include "Hittable.h"

class Camera
{
public:
    static SDL_Renderer *image;
    double aspectRatio = double(imageWidth) / imageHeight;
    int samplesPerPixel = 10;
    int maxDepth = 10;

    void render(const Hittable &world);

private:
    float pixelSamplesScale{};
    Point center;
    Point pixel00Loc;
    Vec3f pixelDeltaU;
    Vec3f pixelDeltaV;

    void initialize();

    [[nodiscard]] Ray getRay(int i, int j) const;

    static Vec3f sampleSquare() { return {randomFloat() - 0.5f, randomFloat() - 0.5f, 0}; };

    static Vec3f rayColor(const Ray &r, int depth, const Hittable &world);
};
#endif//INC_3DRENDERER_CAMERA_H