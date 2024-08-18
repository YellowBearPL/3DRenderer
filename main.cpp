#include "Model.h"
#include "Ray.h"
#include "Sphere.h"
#include "Vec2.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>

const int width = 1920;
const int height = 1080;
const float invWidth = 1 / float(width), invHeight = 1 / float(height);
const float fov = 30, aspectratio = width / float(height);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));

void triangle(const std::vector<Vec2i> &pts, SDL_Renderer *image, SDL_Color color)
{
    Vec2i bboxmin{width - 1, height - 1};
    Vec2i bboxmax{0, 0};
    Vec2i clamp{width - 1, height - 1};
    for (int i = 0; i < 3; i++)
    {
        bboxmin.u = std::max(0, std::min(bboxmin.u, pts[i].u));
        bboxmin.v = std::max(0, std::min(bboxmin.v, pts[i].v));
        bboxmax.u = std::min(clamp.u, std::max(bboxmax.u, pts[i].u));
        bboxmax.v = std::min(clamp.v, std::max(bboxmax.v, pts[i].v));
    }

    Vec2i p;
    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
    for (p.u = bboxmin.u; p.u <= bboxmax.u; p.u++)
    {
        for (p.v = bboxmin.v; p.v <= bboxmax.v; p.v++)
        {
            Vec3f bcScreen  = p.barycentric(pts);
            if (bcScreen.x < 0 || bcScreen.y < 0 || bcScreen.z < 0)
            {
                continue;
            }

            SDL_RenderDrawPoint(image, p.u, p.v);
        }
    }
}

int main()
{
    SDL_Color color;
    std::vector<Vec2i> pts = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};
    SDL_Event event;
    SDL_Renderer *image;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &image);
    SDL_Texture *texture = SDL_CreateTexture(image, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, width, height);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, image);
    ImGui_ImplSDLRenderer2_Init(image);
    std::array<float, 4> sphere{0, 0, -30, 2}, fgColor{0, 1, 0, 1}, bgColor{.01, .01, .01, 1}, lp{0, 1, 0, 1};
    bool glass = false;
    float bias = 1e-4, index = 1.1;
    while (true)
    {
        if (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                break;
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Ray Tracing!");
        if (ImGui::CollapsingHeader("Objects"))
        {
            for (auto k = Ray::objects.begin(); k != Ray::objects.end();)
            {
                if (ImGui::ColorButton(("Delete" + std::to_string(k - Ray::objects.begin())).c_str(), {float(k->get()->color.r) / 255, float(k->get()->color.g) / 255, float(k->get()->color.b) / 255, float(k->get()->color.a) / 255}))
                {
                    Ray::objects.erase(k);
                }
                else
                {
                    k++;
                }
            }

            ImGui::InputFloat3("Center", sphere.data());
            ImGui::InputFloat("Radius", sphere.data() + 3);
            ImGui::ColorPicker4("Color", fgColor.data());
            ImGui::Checkbox("Glass?", &glass);
            if (glass)
            {
                ImGui::InputFloat("index", &index);
            }

            if (ImGui::Button("Add sphere"))
            {
                Ray::objects.push_back(std::make_shared<Sphere>(Point(sphere[0], sphere[1], sphere[2]), sphere[3], SDL_Color(static_cast<Uint8>(fgColor[0] * 255), static_cast<Uint8>(fgColor[1] * 255), static_cast<Uint8>(fgColor[2] * 255), static_cast<Uint8>(fgColor[3] * 255)), glass, index));
            }
        }

        ImGui::ColorPicker4("Background color", bgColor.data());
        ImGui::InputFloat("Bias", &bias, 0.0f, 0.0f, "%.4f");
        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::InputFloat3("Position", lp.data());
            ImGui::SliderFloat("Brightness", lp.data() + 3, 0, 1);
        }

        if (ImGui::Button("Render"))
        {
            Ray::backgroundColor = {static_cast<Uint8>(bgColor[0] * 255), static_cast<Uint8>(bgColor[1] * 255), static_cast<Uint8>(bgColor[2] * 255), static_cast<Uint8>(bgColor[3] * 255)};
            Ray::bias = bias;
            Ray::lightPosition = {lp[0], lp[1], lp[2]};
            Ray::light.brightness = lp[3];
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    Ray primRay;
                    primRay.computePrimRay(i, j);
                    color = primRay.trace(0);
                    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
                    SDL_RenderDrawPoint(image, i, j);
                }
            }
        }

        ImGui::End();
        ImGui::Begin("Rasterization!");
        if (ImGui::Button("Render"))
        {
            SDL_SetRenderTarget(image, texture);
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            triangle(pts, image, {255, 0, 0, 255});
            SDL_SetRenderTarget(image, nullptr);
            SDL_RenderCopyEx(image, texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_VERTICAL);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), image);
        SDL_RenderPresent(image);
    }

    SDL_DestroyRenderer(image);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
