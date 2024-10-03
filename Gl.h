#ifndef INC_3DRENDERER_GL_H
#define INC_3DRENDERER_GL_H
#include <SDL2/SDL.h>
#include "Geometry.h"

void viewport(int x, int y, int w, int h);

void projection(float coeff = 0.f);
#endif//INC_3DRENDERER_GL_H