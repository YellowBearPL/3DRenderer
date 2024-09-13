#ifndef INC_3DRENDERER_RT_H
#define INC_3DRENDERER_RT_H
#include <limits>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

double degreesToRadians(double degrees);

double randomDouble();

double randomDouble(double min, double max);
#endif//INC_3DRENDERER_RT_H