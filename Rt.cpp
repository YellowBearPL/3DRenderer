#include "Rt.h"
#include <random>

double degreesToRadians(double degrees)
{
    return degrees * pi / 180.0;
}

double randomDouble()
{
    static std::uniform_real_distribution<double> distribution{0.0, 1.0};
    static std::mt19937 generator{std::random_device()()};
    return distribution(generator);
}

double randomDouble(double min, double max)
{
    return min + ((max - min) * randomDouble());
}