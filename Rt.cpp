#include "Rt.h"
#include <random>

double degreesToRadians(double degrees)
{
    return degrees * pi / 180.0;
}

float randomFloat()
{
    static std::uniform_real_distribution<float> distribution{0.0, 1.0};
    static std::mt19937 generator{std::random_device()()};
    return distribution(generator);
}

float randomFloat(float min, float max)
{
    return min + ((max - min) * randomFloat());
}