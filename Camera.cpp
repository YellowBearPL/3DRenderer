#include "Camera.h"
#include <iostream>

SDL_Renderer *Camera::image;

SDL_Color operator/(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r / f), static_cast<Uint8>(color.g / f), static_cast<Uint8>(color.b / f), static_cast<Uint8>(color.a / f)};
}

SDL_Color &operator+=(SDL_Color &c1, const SDL_Color &c2)
{
    c1 = c1 + c2;
    return c1;
}

void Camera::render(const Hittable &world)
{
    initialize();
    for (int j = 0; j < imageHeight; j++)
    {
        std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++)
        {
            SDL_Color pixelColor{0, 0, 0, 255};
            for (int sample = 0; sample < samplesPerPixel; sample++)
            {
                Ray r = getRay(i, j);
                pixelColor += rayColor(r, world) / samplesPerPixel;
            }

            SDL_SetRenderDrawColor(image, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
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