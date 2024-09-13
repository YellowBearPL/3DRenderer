#include "Rt.h"
#include <random>

double degreesToRadians(double degrees)
{
    return degrees * pi / 180.0;
}

double randomDouble()
{
    return std::mt19937(std::random_device()())() / (std::mt19937::max() + 1.0);
}

double randomDouble(double min, double max)
{
    return min + ((max - min) * randomDouble());
}