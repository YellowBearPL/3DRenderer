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
#include <filesystem>
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
    std::unique_ptr<Model> model;
    std::mt19937 mt{std::random_device()()};
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
    std::filesystem::directory_iterator objDirectory{"obj/"};
    std::vector<std::string> objFiles;
    for (auto &file : objDirectory)
    {
        objFiles.push_back(file.path().filename().generic_string());
    }

    std::string filename;
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
        for (auto &file : objFiles)
        {
            if (ImGui::RadioButton(file.c_str(), filename == file))
            {
                filename = file;
            }
        }

        if (ImGui::Button("Render"))
        {
            model = std::make_unique<Model>(("obj/" + filename).c_str());
            SDL_SetRenderTarget(image, texture);
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            for (int i = 0; i < model->nfaces(); i++)
            {
                std::vector<int> face = model->face(i);
                Vec2i screen_coords[3];
                for (int j = 0; j < 3; j++)
                {
                    Vec3f world_coords = model->vert(face[j]);
                    screen_coords[j] = {int((world_coords.x + 4.) * width / 8.), int((world_coords.y + 4.) * height / 8.)};
                }

                triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, SDL_Color(mt() % 255, mt() % 255, mt() % 255, 255));
            }

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
