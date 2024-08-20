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
#include <memory>
#include <random>

const int width = 1920;
const int height = 1080;
const float invWidth = 1 / float(width), invHeight = 1 / float(height);
const float fov = 30, aspectratio = width / float(height);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, SDL_Renderer *image, SDL_Color color)
{
    if (t0.v == t1.v && t0.v == t2.v)
    {
        return;
    }

    if (t0.v > t1.v)
    {
        std::swap(t0, t1);
    }

    if (t0.v > t2.v)
    {
        std::swap(t0, t2);
    }

    if (t1.v > t2.v)
    {
        std::swap(t1, t2);
    }

    int totalHeight = t2.v - t0.v;
    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
    for (int i = 0; i < totalHeight; i++)
    {
        bool secondHalf = i > t1.v - t0.v || t1.v == t0.v;
        int segmentHeight = secondHalf ? t2.v - t1.v : t1.v - t0.v;
        auto alpha = float(i) / float(totalHeight);
        auto beta = float(i - (secondHalf ? t1.v - t0.v : 0)) / float(segmentHeight);
        Vec2i a = t0 + ((t2 - t0) * alpha);
        Vec2i b = secondHalf ? t1 + ((t2 - t1) * beta) : t0 + ((t1 - t0) * beta);
        if (a.u > b.u)
        {
            std::swap(a, b);
        }

        for (int j = a.u; j <= b.u; j++)
        {
            SDL_RenderDrawPoint(image, j, t0.v + i);
        }
    }
}

int main()
{
    SDL_Color color;
    SDL_Color red;
    SDL_Color green;
    SDL_Color blue;
    SDL_Color white;
    SDL_Event event;
    SDL_Renderer *scene;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &scene);
    SDL_Texture *texture = SDL_CreateTexture(scene, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, width, height);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, scene);
    ImGui_ImplSDLRenderer2_Init(scene);
    std::array<float, 4> sphere{0, 0, -30, 2}, fgColor{0, 1, 0, 1}, bgColor{.01, .01, .01, 1}, lp{0, 1, 0, 1}, color0{1, 0, 0, 1}, color1{0, 1, 0, 1}, color2{0, 0, 1, 1}, color3{1, 1, 1, 1};
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
            SDL_SetRenderDrawColor(scene, 0, 0, 0, 0);
            SDL_RenderClear(scene);
            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    Ray primRay;
                    primRay.computePrimRay(i, j);
                    color = primRay.trace(0);
                    SDL_SetRenderDrawColor(scene, color.r, color.g, color.b, color.a);
                    SDL_RenderDrawPoint(scene, i, j);
                }
            }
        }

        ImGui::End();
        ImGui::Begin("Rasterization!");
        ImGui::ColorEdit4("Color 0", color0.data());
        ImGui::ColorEdit4("Color 1", color1.data());
        ImGui::ColorEdit4("Color 2", color2.data());
        ImGui::ColorEdit4("Color 3", color3.data());
        if (ImGui::Button("Render"))
        {
            red = {static_cast<Uint8>(color0[0] * 255), static_cast<Uint8>(color0[1] * 255), static_cast<Uint8>(color0[2] * 255), static_cast<Uint8>(color0[3] * 255)};
            green = {static_cast<Uint8>(color1[0] * 255), static_cast<Uint8>(color1[1] * 255), static_cast<Uint8>(color1[2] * 255), static_cast<Uint8>(color1[3] * 255)};
            blue = {static_cast<Uint8>(color2[0] * 255), static_cast<Uint8>(color2[1] * 255), static_cast<Uint8>(color2[2] * 255), static_cast<Uint8>(color2[3] * 255)};
            white = {static_cast<Uint8>(color3[0] * 255), static_cast<Uint8>(color3[1] * 255), static_cast<Uint8>(color3[2] * 255), static_cast<Uint8>(color3[3] * 255)};
            SDL_SetRenderTarget(scene, texture);
            SDL_SetRenderDrawColor(scene, 0, 0, 0, 0);
            SDL_RenderClear(scene);
            Vec2i(20, 34).line({744, 400}, scene, red);
            Vec2i(120, 434).line({444, 400}, scene, green);
            Vec2i(330, 463).line({594, 200}, scene, blue);
            Vec2i(10, 10).line({790, 10}, scene, white);
            SDL_SetRenderTarget(scene, nullptr);
            SDL_RenderCopyEx(scene, texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_VERTICAL);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), scene);
        SDL_RenderPresent(scene);
    }

    SDL_DestroyRenderer(scene);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
