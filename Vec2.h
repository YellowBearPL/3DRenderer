#ifndef INC_3DRENDERER_VEC2_H
#define INC_3DRENDERER_VEC2_H
#include <SDL2/SDL.h>
#include <algorithm>

template<typename T>
class Vec2
{
    T u;

public:
    T v;

    Vec2(T u, T v) : u(u), v(v) {}

    static void line(int x0, int y0, int x1, int y1, SDL_Renderer *image, SDL_Color color) { bool steep = false; if (std::abs(x0 - x1) < std::abs(y0 - y1)) { std::swap(x0, y0); std::swap(x1, y1); steep = true; } if (x0 > x1) { std::swap(x0, x1); std::swap(y0, y1); } int dx = x1 - x0; int dy = y1 - y0; int derror2 = std::abs(dy) * 2; int error2 = 0; int y = y0; SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a); for (int x = x0; x <= x1; x++) { if (steep) { SDL_RenderDrawPoint(image, y, x); } else { SDL_RenderDrawPoint(image, x, y); } error2 += derror2; if (error2 > dx) { y += (y1 > y0 ? 1 : -1); error2 -= dx * 2; }}}

    void line(Vec2<int> p1, SDL_Renderer *image, SDL_Color color) { line(u, v, p1.u, p1.v, image, color); }
};

using Vec2i = Vec2<int>;
#endif//INC_3DRENDERER_VEC2_H