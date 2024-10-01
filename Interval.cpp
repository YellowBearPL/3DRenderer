#include "Interval.h"

const Interval Interval::empty = {+infinity, -infinity};

double Interval::clamp(double x) const
{
    if (x < min)
    {
        return min;
    }

    if (x > max)
    {
        return max;
    }

    return x;
}