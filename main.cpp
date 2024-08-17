#include "Light.h"
#include "Object.h"
#include "Ray.h"
#include "Sphere.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

constexpr int imageHeight = 1080, imageWidth = 1920;
constexpr float invWidth = 1 / float(imageWidth), invHeight = 1 / float(imageHeight);
constexpr float fov = 30, aspectratio = imageWidth / float(imageHeight);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));

SDL_Color operator*(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r * f), static_cast<Uint8>(color.g * f), static_cast<Uint8>(color.b * f), color.a};
}

SDL_Color operator*(const double &f, const SDL_Color &color)
{
    return color * f;
}

SDL_Color operator+(const SDL_Color &v1, const SDL_Color &v2)
{
    return {static_cast<Uint8>(v1.r + v2.r), static_cast<Uint8>(v1.b + v2.b), static_cast<Uint8>(v1.g + v2.g), static_cast<Uint8>(v1.a + v2.a)};
}

SDL_Color computeReflectionColor()
{
    return {};
}

SDL_Color computeRefractionColor()
{
    return {};
}

int main()
{
    std::vector<std::shared_ptr<Object>> objects;
    Point eyePosition;
    Point lightPosition;
    Light light{};
    SDL_Color glassBallColorAtHit;
    float refractiveIndex = 1.1;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(imageWidth, imageHeight, 0, &window, &renderer);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    std::array<float, 4> center{0, 0, -30, 2}, fColor{0, 1, 0, 1}, lp{0, 1, 0, 1};
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
            for (auto k = objects.begin(); k != objects.end();)
            {
                if (ImGui::ColorButton(("Delete" + std::to_string(k - objects.begin())).c_str(), {float(k->get()->color.r) / 255, float(k->get()->color.g) / 255, float(k->get()->color.b) / 255, float(k->get()->color.a) / 255}))
                {
                    objects.erase(k);
                }
                else
                {
                    k++;
                }
            }

            ImGui::InputFloat3("Center", center.data());
            ImGui::InputFloat("Radius", center.data() + 3);
            ImGui::ColorPicker4("Color", fColor.data());
            if (ImGui::Button("Add sphere"))
            {
                objects.push_back(std::make_shared<Sphere>(Point(center[0], center[1], center[2]), center[3], SDL_Color(Uint8(fColor[0] * 255), Uint8(fColor[1] * 255), Uint8(fColor[2] * 255), Uint8(fColor[3] * 255))));
            }
        }

        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::InputFloat3("Position", lp.data());
            ImGui::SameLine();
            ImGui::InputFloat("Brightness", lp.data() + 3);
        }

        if (ImGui::Button("Render"))
        {
            lightPosition = {lp[0], lp[1], lp[2]};
            light.brightness = lp[3];
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            for (int j = 0; j < imageHeight; j++)
            {
                for (int i = 0; i < imageWidth; i++)
                {
                    Ray primRay;
                    primRay.computePrimRay(i, j);
                    Point pHit;
                    Normal normalHit;
                    float minDist = INFINITY;
                    std::shared_ptr<Object> object = nullptr;
                    for (auto &k : objects)
                    {
                        if (k->intersect(primRay, pHit, normalHit))
                        {
                            float distance = eyePosition.distance(pHit);
                            if (distance < minDist)
                            {
                                object = k;
                                minDist = distance;
                            }
                        }
                    }

                    if (object != nullptr)
                    {
                        Ray shadowRay;
                        shadowRay.direction = lightPosition - pHit;
                        SDL_Color reflectionColor = computeReflectionColor();
                        SDL_Color refractionColor = computeRefractionColor();
                        float kr;
                        float kt;
                        Vec3f primaryRayDirection = primRay.direction;
                        normalHit.fresnel(refractiveIndex, primaryRayDirection, kr, kt);
                        glassBallColorAtHit = kr * reflectionColor + kt * refractionColor;
                        SDL_SetRenderDrawColor(renderer, glassBallColorAtHit.r, glassBallColorAtHit.g, glassBallColorAtHit.b, glassBallColorAtHit.a);
                        SDL_RenderDrawPoint(renderer, i, j);
                    }
                }
            }
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
