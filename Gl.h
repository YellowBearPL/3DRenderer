#ifndef INC_3DRENDERER_GL_H
#define INC_3DRENDERER_GL_H
#include <SDL2/SDL.h>
#include "Geometry.h"

extern Matrix mViewport;
extern Matrix mProjection;

void viewport(int x, int y, int w, int h);

void projection(float coeff = 0.f);

class Shader
{
public:
    static float depth;

    virtual ~Shader() = default;

    virtual Vec4f vertex(int iface, int nthvert) = 0;

    virtual bool fragment(Vec3f bar, SDL_Color &color) = 0;

    void triangle(std::vector<Vec4f> pts, SDL_Renderer *image, std::vector<float> &zbuffer);
};
#endif//INC_3DRENDERER_GL_H