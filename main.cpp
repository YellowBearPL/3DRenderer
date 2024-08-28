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
#include <SDL2/SDL_test.h>

#define TEX_SIZE 64
#define MAP_WIDTH 24
#define MAP_HEIGHT 24

std::unique_ptr<Model> model = nullptr;
const int screenWidth = 1920;
const int screenHeight = 1080;
const float invWidth = 1 / float(screenWidth), invHeight = 1 / float(screenHeight);
const float fov = 30, aspectratio = screenWidth / float(screenHeight);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));
Vec3f eye{1, 1, 3};
Vec3f center{0, 0, 0};
Vec3f up{0, 1, 0};
std::vector<std::vector<int>> worldMap =
        {
                {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7},
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
                {4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
                {4, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
                {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
                {4, 0, 4, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 0, 7, 7, 7, 7, 7},
                {4, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
                {4, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
                {4, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 1},
                {4, 0, 8, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
                {4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
                {4, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 1},
                {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
                {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                {6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
                {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 6, 0, 6, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3},
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
                {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2},
                {4, 0, 0, 5, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
                {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
                {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
                {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3}
        };

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

SDL_Color operator/(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r / f), static_cast<Uint8>(color.g / f), static_cast<Uint8>(color.b / f), color.a};
}

SDL_Color &operator/=(SDL_Color &color, const double &f)
{
    color = color / f;
    return color;
}

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
    viewport(screenWidth / 8, screenHeight / 8, screenWidth * 3 / 4, screenHeight * 3 / 4);
    projection(-1.f / (eye - center).norm());
    std::vector<Vec4f> screenCoords(3);
    std::vector<float> zbuffer(screenWidth * screenHeight);
    for (int i = screenWidth * screenHeight; --i;)
    {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    ZShader zshader;
    double posX = 22.0, posY = 11.5;
    double dirX = -1.0, dirY = 0.0;
    double planeX = 0.0, planeY = 0.66;
    double time = 0;
    double oldTime = 0;
    std::vector<Uint32> buffer(screenHeight * screenWidth);
    std::array<std::vector<int>, 8> texture;
    for (int i = 0; i < 8; i++)
    {
        texture[i].resize(TEX_SIZE * TEX_SIZE);
    }

    for (int x = 0; x < TEX_SIZE; x++)
    {
        for (int y = 0; y < TEX_SIZE; y++)
        {
            int xorcolor = (x * 256 / TEX_SIZE) ^ (y * 256 / TEX_SIZE);
            int ycolor = y * 256 / TEX_SIZE;
            int xycolor = (y * 128 / TEX_SIZE) + (x * 128 / TEX_SIZE);
            texture[0][(TEX_SIZE * y) + x] = 65536 * 254 * (x != y && x != TEX_SIZE - y);
            texture[1][(TEX_SIZE * y) + x] = xycolor + (256 * xycolor) + (65536 * xycolor);
            texture[2][(TEX_SIZE * y) + x] = (256 * xycolor) + (65536 * xycolor);
            texture[3][(TEX_SIZE * y) + x] = xorcolor + (256 * xorcolor) + (65536 * xorcolor);
            texture[4][(TEX_SIZE * y) + x] = 256 * xorcolor;
            texture[5][(TEX_SIZE * y) + x] = 65536 * 192 * (x % 16 && y % 16);
            texture[6][(TEX_SIZE * y) + x] = 65536 * ycolor;
            texture[7][(TEX_SIZE * y) + x] = 128 + (256 * 128) + (65536 * 128);
        }
    }

    for (size_t i = 0; i < 8; i++)
    {
        for (size_t x = 0; x < TEX_SIZE; x++)
        {
            for (size_t y = 0; y < x; y++)
            {
                std::swap(texture[i][(TEX_SIZE * y) + x], texture[i][(TEX_SIZE * x) + y]);
            }
        }
    }

    SDL_Surface *srf = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_Event event;
    SDL_Renderer *image;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &window, &image);
    SDL_SetWindowTitle(window, "3D Renderer!");
    SDL_Texture *frame = SDL_CreateTexture(image, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, image);
    ImGui_ImplSDLRenderer2_Init(image);
    std::array<float, 4> sphere{0, 0, -30, 2}, fgColor{0, 1, 0, 1}, bgColor{.01, .01, .01, 1}, lp{0, 1, 0, 1};
    bool glass = false, raycast = false;
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
            for (int j = 0; j < screenHeight; j++)
            {
                for (int i = 0; i < screenWidth; i++)
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

            for (int x = 0; x < screenWidth; x++)
            {
                for (int y = 0; y < screenHeight; y++)
                {
                    if (zbuffer[x + (y * screenWidth)] < -1e5)
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
        ImGui::Begin("Ray Casting!");
        ImGui::Checkbox("Render", &raycast);
        if (raycast)
        {
            SDL_SetRenderDrawColor(image, 0, 0, 0, 0);
            SDL_RenderClear(image);
            for (int x = 0; x < screenWidth; x++)
            {
                double cameraX = (2 * x / double(screenWidth)) - 1;
                double rayDirX = dirX + (planeX * cameraX);
                double rayDirY = dirY + (planeY * cameraX);
                auto mapX = int(posX);
                auto mapY = int(posY);
                double sideDistX;
                double sideDistY;
                double deltaDistX = sqrt(1 + ((rayDirY * rayDirY) / (rayDirX * rayDirX)));
                double deltaDistY = sqrt(1 + ((rayDirX * rayDirX) / (rayDirY * rayDirY)));
                double perpWallDist;
                int stepX;
                int stepY;
                int hit = 0;
                int side;
                if (rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (posX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - posX) * deltaDistX;
                }

                if (rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (posY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - posY) * deltaDistY;
                }

                while (hit == 0)
                {
                    if (sideDistX < sideDistY)
                    {
                        sideDistX += deltaDistX;
                        mapX += stepX;
                        side = 0;
                    }
                    else
                    {
                        sideDistY += deltaDistY;
                        mapY += stepY;
                        side = 1;
                    }

                    if (worldMap[mapX][mapY] > 0)
                    {
                        hit = 1;
                    }
                }

                if (side == 0)
                {
                    perpWallDist = (sideDistX - deltaDistX);
                }
                else
                {
                    perpWallDist = (sideDistY - deltaDistY);
                }

                auto lineHeight = int(screenHeight / perpWallDist);
                int drawStart = (-lineHeight / 2) + (screenHeight / 2);
                if (drawStart < 0)
                {
                    drawStart = 0;
                }

                int drawEnd = (lineHeight / 2) + (screenHeight / 2);
                if (drawEnd >= screenHeight)
                {
                    drawEnd = screenHeight - 1;
                }

                int texNum = worldMap[mapX][mapY] - 1;
                double wallX;
                if (side == 0)
                {
                    wallX = posY + (perpWallDist * rayDirY);
                }
                else
                {
                    wallX = posX + (perpWallDist * rayDirX);
                }

                wallX -= floor((wallX));
                auto texX = int(wallX * double(TEX_SIZE));
                if (side == 0 && rayDirX > 0)
                {
                    texX = TEX_SIZE - texX - 1;
                }

                if (side == 1 && rayDirY < 0)
                {
                    texX = TEX_SIZE - texX - 1;
                }

                double step = 1.0 * TEX_SIZE / lineHeight;
                double texPos = (drawStart - (screenHeight / 2.) + (lineHeight / 2.)) * step;
                for (int y = drawStart; y < drawEnd; y++)
                {
                    auto texY = int(texPos) & (TEX_SIZE - 1);
                    texPos += step;
                    Uint32 uColor = texture[texNum][(TEX_SIZE * texY) + texX];
                    if (side == 1)
                    {
                        uColor = (uColor >> 1) & 8355711;
                    }

                    buffer[(screenWidth * y) + x] = uColor;
                }
            }

            auto *bufp = (Uint32 *)srf->pixels;
            for (int y = 0; y < screenHeight; y++)
            {
                for (int x = 0; x < screenWidth; x++)
                {
                    *bufp = buffer[(y * screenWidth) + x];
                    bufp++;
                }

                bufp += srf->pitch / 4;
                bufp -= screenWidth;
            }

            for (int y = 0; y < screenHeight; y++)
            {
                for (int x = 0; x < screenWidth; x++)
                {
                    buffer[(screenWidth * y) + x] = 0;
                }
            }

            oldTime = time;
            time = double(SDL_GetTicks64());
            double frameTime = (time - oldTime) / 1000.0;
            SDL_UpdateTexture(frame, nullptr, srf->pixels, srf->pitch);
            SDL_RenderCopy(image, frame, nullptr, nullptr);
            SDL_SetRenderDrawColor(image, 255, 255, 255, 255);
            SDLTest_DrawString(image, 0, 0, std::to_string(1.0 / frameTime).c_str());
            double moveSpeed = frameTime * 5.0;
            double rotSpeed = frameTime * 3.0;
            const Uint8* keystate = SDL_GetKeyboardState(nullptr);
            if (keystate[SDL_SCANCODE_UP])
            {
                if (worldMap[int(posX + (dirX * moveSpeed))][int(posY)] == false)
                {
                    posX += dirX * moveSpeed;
                }

                if (worldMap[int(posX)][int(posY + (dirY * moveSpeed))] == false)
                {
                    posY += dirY * moveSpeed;
                }
            }

            if (keystate[SDL_SCANCODE_DOWN])
            {
                if (worldMap[int(posX - (dirX * moveSpeed))][int(posY)] == false)
                {
                    posX -= dirX * moveSpeed;
                }

                if (worldMap[int(posX)][int(posY - (dirY * moveSpeed))] == false)
                {
                    posY -= dirY * moveSpeed;
                }
            }

            if (keystate[SDL_SCANCODE_RIGHT])
            {
                double oldDirX = dirX;
                dirX = (dirX * cos(-rotSpeed)) - (dirY * sin(-rotSpeed));
                dirY = (oldDirX * sin(-rotSpeed)) + (dirY * cos(-rotSpeed));
                double oldPlaneX = planeX;
                planeX = (planeX * cos(-rotSpeed)) - (planeY * sin(-rotSpeed));
                planeY = (oldPlaneX * sin(-rotSpeed)) + (planeY * cos(-rotSpeed));
            }

            if (keystate[SDL_SCANCODE_LEFT])
            {
                double oldDirX = dirX;
                dirX = (dirX * cos(rotSpeed)) - (dirY * sin(rotSpeed));
                dirY = (oldDirX * sin(rotSpeed)) + (dirY * cos(rotSpeed));
                double oldPlaneX = planeX;
                planeX = (planeX * cos(rotSpeed)) - (planeY * sin(rotSpeed));
                planeY = (oldPlaneX * sin(rotSpeed)) + (planeY * cos(rotSpeed));
            }
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
