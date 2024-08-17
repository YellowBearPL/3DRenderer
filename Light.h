#ifndef INC_3DRENDERER_LIGHT_H
#define INC_3DRENDERER_LIGHT_H
#include <SDL2/SDL.h>

class Light
{
    SDL_Color color;

public:
    Light(SDL_Color color, double brightness) : color(color), brightness(brightness) {}
    Light() = default;

    double brightness;
};
#endif//INC_3DRENDERER_LIGHT_H