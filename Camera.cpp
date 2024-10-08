#include "Camera.h"
#include <iostream>
#include "Material.h"

SDL_Renderer *Camera::image;

Vec3f operator*(const Vec3f &u, const Vec3f &v) {
    return {u.x * v.x, u.y * v.y, u.z * v.z};
}

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
                pixelColor += rayColor(r, maxDepth, world);
            }

            pixelColor *= pixelSamplesScale;
            static const Interval intensity{0.000, 0.999};
            SDL_SetRenderDrawColor(image, int(256 * intensity.clamp(linearToGamma(pixelColor.x))), int(256 * intensity.clamp(linearToGamma(pixelColor.y))), int(256 * intensity.clamp(linearToGamma(pixelColor.z))), 255);
            SDL_RenderDrawPoint(image, i, j);
        }
    }

    std::clog << "\rDone.                 " << std::endl;
}

void Camera::initialize()
{
    pixelSamplesScale = float(1.0 / samplesPerPixel);
    center = lookfrom;
    double theta = degreesToRadians(vfov);
    auto h = float(std::tan(theta / 2));
    float viewportHeight = 2.f * h * focusDist;
    float viewportWidth = viewportHeight * aspectRatio;
    w = (lookfrom - lookat).unitVector();
    u = vup.cross(w).unitVector();
    v = w.cross(u);
    Vec3f viewportU = viewportWidth * u;
    Vec3f viewportV = viewportHeight * -v;
    pixelDeltaU = viewportU / float(imageWidth);
    pixelDeltaV = viewportV / float(imageHeight);
    Vec3f viewportUpperLeft = center - (focusDist * w) - (viewportU / 2) - (viewportV / 2);
    pixel00Loc = viewportUpperLeft + (0.5f * (pixelDeltaU + pixelDeltaV));
    auto defocusRadius = float(focusDist * std::tan(degreesToRadians(defocusAngle / 2)));
    defocusDiskU = u * defocusRadius;
    defocusDiskV = v * defocusRadius;
}

Ray Camera::getRay(int i, int j) const
{
    Vec3f offset = sampleSquare();
    Vec3f pixelSample = pixel00Loc + ((float(i) + offset.x) * pixelDeltaU) + ((float(j) + offset.y) * pixelDeltaV);
    Point rayOrigin = (defocusAngle <= 0) ? center : defocusDiskSample();
    Vec3f rayDirection = pixelSample - rayOrigin;
    return {rayOrigin, rayDirection};
}

Point Camera::defocusDiskSample() const
{
    Vec3f p = Vec3f::randomInUnitDisk();
    return center + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
}

Vec3f Camera::rayColor(const Ray &r, int depth, const Hittable &world)
{
    if (depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;
    if (world.hit(r, {0.001, infinity}, rec))
    {
        Ray scattered;
        Vec3f attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * rayColor(scattered, depth - 1, world);
        }

        return {0, 0, 0};
    }

    Vec3f unitDirection = r.dir.unitVector();
    double a = 0.5 * (unitDirection.y + 1.0);
    return (float(1.0 - a) * Vec3f(1.0, 1.0, 1.0)) + (float(a) * Vec3f(0.5, 0.7, 1.0));
}

double Camera::linearToGamma(double linearComponent)
{
    if (linearComponent > 0)
    {
        return std::sqrt(linearComponent);
    }

    return 0;
}