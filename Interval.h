#ifndef INC_3DRENDERER_INTERVAL_H
#define INC_3DRENDERER_INTERVAL_H
#include "Rt.h"

class Interval
{
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {}

    Interval(double min, double max) : min(min), max(max) {}

    [[nodiscard]] double size() const { return max - min; }

    [[nodiscard]] bool contains(double x) const { return min <= x && x <= max; }

    [[nodiscard]] bool surrounds(double x) const { return min < x && x < max; }

    static const Interval empty, universe;
};
#endif//INC_3DRENDERER_INTERVAL_H