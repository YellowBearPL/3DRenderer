#include "Camera.h"
#include <iostream>

SDL_Renderer *Camera::image;

void Camera::render(const Hittable &world)
{
    initialize();
    SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
    SDL_RenderClear(image);
    for (int j = 0; j < imageHeight; j++)
    {
        std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            Vec3f pixelCenter = pixel00Loc + (float(i) * pixelDeltaU) + (float(j) * pixelDeltaV);
            Vec3f rayDirection = pixelCenter - center;
            Ray r{center, rayDirection};
            SDL_Color pixelColor = r.rayColor(world);
            SDL_SetRenderDrawColor(image, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
            SDL_RenderDrawPoint(image, i, j);
        }
    }

    std::clog << "\rDone.                 " << std::endl;
}

void Camera::initialize()
{
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

SDL_Color Camera::rayColor(const Ray &r, const Hittable &world)
{
    HitRecord rec;
    if (world.hit(r, {0, infinity}, rec))
    {
        return {static_cast<Uint8>((rec.normal.x + 1) * 127.5), static_cast<Uint8>((rec.normal.y + 1) * 127.5), static_cast<Uint8>((rec.normal.z + 1) * 127.5), 255};
    }

    Vec3f unitDirection = r.dir.unitVector();
    double a = 0.5 * (unitDirection.y + 1.0);
    return ((1.0 - a) * SDL_Color(255, 255, 255, 255)) + (a * SDL_Color(127, 178, 255, 255));
}