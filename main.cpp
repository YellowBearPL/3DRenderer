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
Matrix modelView;
const int depth = 255;

void triangle(std::vector<Vec3f> pts, std::vector<float> zbuffer, SDL_Renderer *image, SDL_Color color)
{
    Vec2f bboxmin{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vec2f bboxmax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
    Vec2f clamp{width - 1, height - 1};
    for (int i = 0; i < 3; i++)
    {
        bboxmin.u = std::max(0.f, std::min(bboxmin.u, pts[i].x));
        bboxmax.u = std::min(clamp.u, std::max(bboxmax.u, pts[i].x));
        bboxmin.v = std::max(0.f, std::min(bboxmin.v, pts[i].y));
        bboxmax.v = std::min(clamp.v, std::max(bboxmax.v, pts[i].y));
    }

    Vec3f p;
    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
    for (p.x = bboxmin.u; p.x <= bboxmax.u; p.x++)
    {
        for (p.y = bboxmin.v; p.y <= bboxmax.v; p.y++)
        {
            Vec3f bcScreen = pts[0].barycentric(pts[1], pts[2], p);
            if (bcScreen.x < 0 || bcScreen.y < 0 || bcScreen.z < 0)
            {
                continue;
            }

            p.z = 0;
            p.z += pts[0].z * bcScreen.x;
            p.z += pts[1].z * bcScreen.y;
            p.z += pts[2].z * bcScreen.z;
            if (zbuffer[int(p.x + (p.y * width))] < p.z)
            {
                zbuffer[int(p.x + (p.y * width))] = p.z;
                SDL_RenderDrawPoint(image, int(p.x), int(p.y));
            }
        }
    }
}

void rasterize(Vec2i p0, Vec2i p1, SDL_Renderer *image, SDL_Color color, std::vector<int> ybuffer) {
    if (p0.u > p1.u)
    {
        std::swap(p0, p1);
    }

    SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
    for (int x = p0.u; x <= p1.u; x++)
    {
        auto t = float(x - p0.u) / float(p1.u - p0.u);
        int y = lround((p0.v * (1. - t)) + (float(p1.v) * t) + .5);
        if (ybuffer[x] < y)
        {
            ybuffer[x] = y;
            SDL_RenderDrawPoint(image, x, 0);
        }
    }
}

Matrix viewport(int x, int y, int w, int h)
{
    Matrix m = Matrix::identity();
    m[0][3] = float(x) + float(w) / 2.f;
    m[1][3] = float(y) + float(h) / 2.f;
    m[2][3] = depth / 2.f;
    m[0][0] = float(w) / 2.f;
    m[1][1] = float(h) / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

int main()
{
    SDL_Color color;
    std::vector<float> zbuffer(width * height, -std::numeric_limits<float>::max());
    std::unique_ptr<Model> model;
    Vec3f lightDir{0, 0, -1};
    Vec3f eye;
    Vec3f center;
    Matrix projection;
    Matrix viewPort;
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
    std::array<float, 3> ep{1, 1, 3}, cp{0, 1, 0};
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

        ImGui::InputFloat3("Eye", ep.data());
        ImGui::InputFloat3("Center", cp.data());
        if (ImGui::Button("Render"))
        {
            model = std::make_unique<Model>(("obj/" + filename).c_str());
            eye = {ep[0], ep[1], ep[2]};
            center = {cp[0], cp[1], cp[2]};
            eye.lookat(center, {0, 1, 0});
            projection = Matrix::identity();
            viewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
            projection[3][2] = -1.f / (eye - center).norm();
            SDL_SetRenderTarget(image, texture);
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            for (int i = 0; i < model->nfaces(); i++)
            {
                std::vector<int> face = model->face(i);
                std::vector<Vec3f> screenCoords(3);
                for (int j = 0; j < 3; j++)
                {
                    Vec3f v = model->vert(face[j]);
                    screenCoords[j] = Vec3f(viewPort * projection * modelView * Matrix(v));
                }

                Vec3f n = (model->vert(face[2]) - model->vert(face[0])) ^ (model->vert(face[1]) - model->vert(face[0]));
                n.normalize();
                float intensity = n * lightDir;
                if (intensity > 0)
                {
                    triangle(screenCoords, zbuffer, image, SDL_Color(Uint8(intensity * 255), Uint8(intensity * 255), Uint8(intensity * 255), 255));
                }
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
