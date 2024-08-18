#ifndef INC_3DRENDERER_VEC2_H
#define INC_3DRENDERER_VEC2_H
#include <SDL2/SDL.h>

template<typename T>
class Vec2
{
    T u, v;

public:
    Vec2(T u, T v) : u(u), v(v) {}

    void line(Vec2<int> p1, SDL_Renderer *image, SDL_Color color) { extern void line(int x0, int y0, int x1, int y1, SDL_Renderer *image, SDL_Color color); line(u, v, p1.u, p1.v, image, color); }

    void triangle(Vec2<int> t1, Vec2<int> t2, SDL_Renderer *image, SDL_Color color) { line(t1, image, color); t1.line(t2, image, color); t2.line(*this, image, color); }
};

using Vec2i = Vec2<int>;
#endif//INC_3DRENDERER_VEC2_H