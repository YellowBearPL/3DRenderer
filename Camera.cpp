#include "Camera.h"
#include <iostream>

SDL_Renderer *Camera::image;

void Camera::render(const Hittable &world)
{
    initialize();
    for (int j = 0; j < imageHeight; j++)
    {
        std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            Vec3f pixelColor{0, 0, 0};
            for (int sample = 0; sample < samplesPerPixel; sample++)
            {
                Ray r = getRay(i, j);
                pixelColor += rayColor(r, world);
            }

            pixelColor *= pixelSamplesScale;
            static const Interval intensity{0.000, 0.999};
            SDL_SetRenderDrawColor(image, int(256 * intensity.clamp(pixelColor.x)), int(256 * intensity.clamp(pixelColor.y)), int(256 * intensity.clamp(pixelColor.z)), 255);
            SDL_RenderDrawPoint(image, i, j);
        }
    }

    std::clog << "\rDone.                 " << std::endl;
}

void Camera::initialize()
{
    pixelSamplesScale = 1.0 / samplesPerPixel;
    center = {0, 0, 0};
    double focalLength = 1.0;
    double viewportHeight = 2.0;
    double viewportWidth = viewportHeight * aspectRatio;
    Vec3f viewportU{static_cast<float>(viewportWidth), 0, 0};
    Vec3f viewportV{0, static_cast<float>(-viewportHeight), 0};
    pixelDeltaU = viewportU / float(imageWidth);
    pixelDeltaV = viewportV / float(imageHeight);
    Vec3f viewportUpperLeft = center - Vec3f(0, 0, float(focalLength)) - (viewportU / 2) - (viewportV / 2);
    pixel00Loc = viewportUpperLeft + (0.5f * (pixelDeltaU + pixelDeltaV));
}

Ray Camera::getRay(int i, int j) const
{
    Vec3f offset = sampleSquare();
    Vec3f pixelSample = pixel00Loc + ((float(i) + offset.x) * pixelDeltaU) + ((float(j) + offset.y) * pixelDeltaV);
    Point rayOrigin = center;
    Vec3f rayDirection = pixelSample - rayOrigin;
    return {rayOrigin, rayDirection};
}

Vec3f Camera::rayColor(const Ray &r, const Hittable &world)
{
    HitRecord rec;
    if (world.hit(r, {0, infinity}, rec))
    {
        return 0.5f * (rec.normal + Vec3f(1, 1, 1));
    }

    Vec3f unitDirection = r.dir.unitVector();
    double a = 0.5 * (unitDirection.y + 1.0);
    return (float(1.0 - a) * Vec3f(1.0, 1.0, 1.0)) + (float(a) * Vec3f(0.5, 0.7, 1.0));
}