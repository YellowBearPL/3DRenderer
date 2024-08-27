#include "Geometry.h"
#include "Gl.h"
#include "Model.h"
#include "Ray.h"
#include "Sphere.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <array>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

std::unique_ptr<Model> model = nullptr;
const int width = 1920;
const int height = 1080;
const float invWidth = 1 / float(width), invHeight = 1 / float(height);
const float fov = 30, aspectratio = width / float(height);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));
Vec3f lightDir{1, 1, 1};
Vec3f eye{1, 1, 3};
Vec3f center{0, 0, 0};
Vec3f up{0, 1, 0};

extern Matrix modelView;

class ZShader : public Shader
{
public:
    Mat43<float> varyingTri;

    Vec4f vertex(int iface, int nthvert) override
    {
        Vec4f glVertex = mProjection * modelView * model->vert(iface, nthvert).embed4();
        varyingTri.setCol(nthvert, glVertex);
        return glVertex;
    }

    bool fragment(Vec3f glFragCoord, Vec3f bar, SDL_Color &color) override
    {
        color = SDL_Color(0, 0, 0, 255);
        return false;
    }
};

int main(int argc, char *argv[])
{
    SDL_Color color;
    if (argc == 2)
    {
        model = std::make_unique<Model>(argv[1]);
    }
    else
    {
        model = std::make_unique<Model>("obj/teapot.obj");
    }

    eye.lookat(center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    std::vector<Vec4f> screenCoords(3);
    std::vector<float> zbuffer(width * height);
    for (int i = width * height; --i;)
    {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    ZShader zshader;
    SDL_Event event;
    SDL_Renderer *image;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &image);
    SDL_Texture *frame = SDL_CreateTexture(image, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, width, height);
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
        ImGui::InputFloat("Depth", &Shader::depth);
        if (ImGui::Button("Render"))
        {
            SDL_SetRenderTarget(image, frame);
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            for (int i = 0; i < model->nfaces(); i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    zshader.vertex(i, j);
                }

                zshader.triangle(zshader.varyingTri, image, zbuffer);
            }

            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    if (zbuffer[x + (y * width)] < -1e5)
                    {
                        continue;
                    }

                    float total = 0;
                    for (float a = 0; a < (M_PI * 2) - 1e-4; a += M_PI / 4)
                    {
                        total += float(M_PI / 2) - Vec2f(float(x), float(y)).maxElevationAngle(zbuffer, Vec2f(std::cos(a), std::sin(a)));
                    }

                    total /= (M_PI / 2) * 8;
                    total = std::pow(total, 100.f);
                    SDL_SetRenderDrawColor(image, Uint8(total * 255), Uint8(total * 255), Uint8(total * 255), 255);
                    SDL_RenderDrawPoint(image, x, y);
                }
            }

            SDL_SetRenderTarget(image, nullptr);
            SDL_RenderCopyEx(image, frame, nullptr, nullptr, 0, nullptr, SDL_FLIP_VERTICAL);
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
