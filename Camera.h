#ifndef INC_3DRENDERER_CAMERA_H
#define INC_3DRENDERER_CAMERA_H
#include "Hittable.h"

Vec3f operator*(const Vec3f &u, const Vec3f &v);

class Camera
{
public:
    static SDL_Renderer *image;
    float aspectRatio = float(imageWidth) / float(imageHeight);
    int samplesPerPixel = 10;
    int maxDepth = 10;
    double vfov = 90;
    Point lookfrom{0, 0, 0};
    Point lookat{0, 0, -1};
    Vec3f vup{0, 1, 0};
    double defocusAngle = 0;
    float focusDist = 10;

    void render(const Hittable &world);

private:
    float pixelSamplesScale{};
    Point center;
    Point pixel00Loc;
    Vec3f pixelDeltaU;
    Vec3f pixelDeltaV;
    Vec3f u, v, w;
    Vec3f defocusDiskU;
    Vec3f defocusDiskV;

    void initialize();

    [[nodiscard]] Ray getRay(int i, int j) const;

    static Vec3f sampleSquare() { return {randomFloat() - 0.5f, randomFloat() - 0.5f, 0}; };

    [[nodiscard]] Point defocusDiskSample() const;

    static Vec3f rayColor(const Ray &r, int depth, const Hittable &world);

    static double linearToGamma(double linearComponent);
};
#endif//INC_3DRENDERER_CAMERA_H