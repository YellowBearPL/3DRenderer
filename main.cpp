#include "Camera.h"
#include "Geometry.h"
#include "Gl.h"
#include "HittableList.h"
#include "Material.h"
#include "Model.h"
#include "Ray.h"
#include "Sphere.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_test.h>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#define TEX_WIDTH 64
#define TEX_HEIGHT 64
#define NUM_SPRITES 19
#define U_DIV 1
#define V_DIV 1
#define V_MOVE 0.0

std::unique_ptr<Model> model = nullptr;
const int imageWidth = 1920;
const int imageHeight = 1080;
const float fov = 30, aspectratio = imageWidth / float(imageHeight);
const float angle = float(tan(M_PI * 0.5 * fov / 180.));
Vec3f up{0, 1, 0};
std::vector<std::vector<int>> worldMap =
        {
                {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
                {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
                {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
                {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
                {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
                {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
                {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
                {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
                {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
                {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
                {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
                {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
                {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
                {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}
        };

class Sprite
{
public:
    double x;
    double y;
    int texture;

    Sprite(double x, double y, int texture) : x(x), y(y), texture(texture) {}
};

std::array<Sprite, NUM_SPRITES> sprite =
        {{
                {20.5, 11.5, 10},
                {18.5, 4.5, 10},
                {10.0, 4.5, 10},
                {10.0, 12.5, 10},
                {3.5, 6.5, 10},
                {3.5, 20.5, 10},
                {3.5, 14.5, 10},
                {14.5, 20.5, 10},
                {18.5, 10.5, 9},
                {18.5, 11.5, 9},
                {18.5, 12.5, 9},
                {21.5, 1.5, 8},
                {15.5, 1.5, 8},
                {16.0, 1.8, 8},
                {16.2, 1.2, 8},
                {3.5, 2.5, 8},
                {9.5, 15.5, 8},
                {10.0, 15.1, 8},
                {10.5, 15.8, 8},
        }};
std::vector<Uint32> buffer(imageHeight * imageWidth);
std::array<double, imageWidth> zBuffer;
std::vector<int> spriteOrder(NUM_SPRITES);
std::vector<double> spriteDistance(NUM_SPRITES);

SDL_Color operator/(const SDL_Color &color, const double &f)
{
    return {static_cast<Uint8>(color.r / f), static_cast<Uint8>(color.g / f), static_cast<Uint8>(color.b / f), static_cast<Uint8>(color.a / f)};
}

SDL_Color &operator/=(SDL_Color &color, const double &f)
{
    color = color / f;
    return color;
}

SDL_Color operator-(const SDL_Color &c1, const SDL_Color &c2)
{
    return {static_cast<Uint8>(c1.r - c2.r), static_cast<Uint8>(c1.g - c2.g), static_cast<Uint8>(c1.b - c2.b), static_cast<Uint8>(c1.a - c2.a)};
}

void sortSprites(std::vector<int> &order, std::vector<double> &dist, int amount)
{
    std::vector<std::pair<double, int>> sprites(amount);
    for (int i = 0; i < amount; i++)
    {
        sprites[i].first = dist[i];
        sprites[i].second = order[i];
    }

    std::sort(sprites.begin(), sprites.end());
    for (int i = 0; i < amount; i++)
    {
        dist[i] = sprites[amount - i - 1].first;
        order[i] = sprites[amount - i - 1].second;
    }
}

Uint32 rgbToInt(const SDL_Color& colorRgba)
{
    return (colorRgba.b | (colorRgba.g << 8) | (colorRgba.r << 16) | (colorRgba.a << 24));
}

SDL_Color intToRgb(Uint32 colorInt)
{
    SDL_Color ColorRGBA;
    ColorRGBA.a = (colorInt & 0xFF000000) >> 24;
    ColorRGBA.r = (colorInt & 0x00FF0000) >> 16;
    ColorRGBA.g = (colorInt & 0x0000FF00) >> 8;
    ColorRGBA.b = (colorInt & 0x000000FF);
    return ColorRGBA;
}

int main()
{
    double posX = 22.0, posY = 11.5;
    double dirX = -1.0, dirY = 0.0;
    double planeX = 0.0, planeY = 0.66;
    double time = 0;
    double oldTime = 0;
    std::array<SDL_Surface *, 11> texture{};
    unsigned error = 0;
    for (int i = 0; i < 8; i++)
    {
        if (!(texture[i] = SDL_LoadBMP((std::string("pics/texture") + std::to_string(i) + ".bmp").c_str())))
        {
            error++;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (!(texture[i + 8] = SDL_LoadBMP((std::string("pics/sprite") + std::to_string(i) + ".bmp").c_str())))
        {
            error++;
        }
    }

    if (error)
    {
        std::cout << "error loading images" << std::endl;
        return 1;
    }

    Uint8 *ptr;
    SDL_Surface *srf = SDL_CreateRGBSurface(0, imageWidth, imageHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    srf->pixels = buffer.data();
    HittableList world;
    std::shared_ptr<Lambertian> groundMaterial = std::make_shared<Lambertian>(Vec3f(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, groundMaterial));
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float chooseMat = randomFloat();
            Point center{static_cast<float>(a + (0.9 * randomFloat())), 0.2, static_cast<float>(b + (0.9 * randomFloat()))};
            if ((center - Point(4, 0.2, 0)).length() > 0.9)
            {
                std::shared_ptr<Material> sphereMaterial;
                if (chooseMat < 0.8)
                {
                    Vec3f albedo = Vec3f::random() * Vec3f::random();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.95)
                {
                    Vec3f albedo = Vec3f::random(0.5, 1);
                    float fuzz = randomFloat(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    std::shared_ptr<Dielectric> material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point(0, 1, 0), 1.0, material1));
    std::shared_ptr<Lambertian> material2 = std::make_shared<Lambertian>(Vec3f(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point(-4, 1, 0), 1.0, material2));
    std::shared_ptr<Metal> material3 = std::make_shared<Metal>(Vec3f(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point(4, 1, 0), 1.0, material3));
    Camera cam;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 10;
    cam.vfov = 20;
    cam.lookfrom = {13, 2, 3};
    cam.lookat = {0, 0, 0};
    cam.vup = {0, 1, 0};
    cam.defocusAngle = 0.6;
    cam.focusDist = 10.0;
    Vec3f v2 = {-48, -10, 82};
    Vec3f v1 = {29, -15, 44};
    Vec3f v0 = {13, 34, 114};
    SDL_Color c0 = {255, 0, 0, 255};
    SDL_Color c1 = {0, 255, 0, 255};
    SDL_Color c2 = {0, 0, 255, 255};
    v0.x /= v0.z;
    v0.y /= v0.z;
    v1.x /= v1.z;
    v1.y /= v1.z;
    v2.x /= v2.z;
    v2.y /= v2.z;
    v0.x = (1 + v0[0]) * 0.5f * imageWidth;
    v0[1] = (1 + v0[1]) * 0.5f * imageHeight;
    v1.x = (1 + v1[0]) * 0.5f * imageWidth;
    v1[1] = (1 + v1[1]) * 0.5f * imageHeight;
    v2.x = (1 + v2[0]) * 0.5f * imageWidth;
    v2[1] = (1 + v2[1]) * 0.5f * imageHeight;
    float area = v0.edgeFunction(v1, v2);
    SDL_Event event;
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(imageWidth, imageHeight, 0, &window, &Camera::image);
    SDL_SetWindowTitle(window, "3D Renderer!");
    SDL_Texture *frame = SDL_CreateTexture(Camera::image, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, imageWidth, imageHeight);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, Camera::image);
    ImGui_ImplSDLRenderer2_Init(Camera::image);
    bool raycast = false;
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
        if (ImGui::Button("Render"))
        {
            SDL_SetRenderDrawColor(Camera::image, 0, 0, 0, 0);
            SDL_RenderClear(Camera::image);
            cam.render(world);
        }

        ImGui::End();
        ImGui::Begin("Rasterization!");
        if (ImGui::Button("Render"))
        {
            SDL_SetRenderDrawColor(Camera::image, 0, 0, 0, 0);
            SDL_RenderClear(Camera::image);
            for (int j = 0; j < imageHeight; j++)
            {
                for (int i = 0; i < imageWidth; i++)
                {
                    Vec3f p = {static_cast<float>(i + 0.5), static_cast<float>(imageHeight - j + 0.5), 0};
                    float w0 = v1.edgeFunction(v2, p);
                    float w1 = v2.edgeFunction(v0, p);
                    float w2 = v0.edgeFunction(v1, p);
                    if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                    {
                        w0 /= area;
                        w1 /= area;
                        w2 /= area;
                        float r = (w0 * float(c0.r)) + (w1 * float(c1.r)) + (w2 * float(c2.r));
                        float g = (w0 * float(c0.g)) + (w1 * float(c1.g)) + (w2 * float(c2.g));
                        float b = (w0 * float(c0.b)) + (w1 * float(c1.b)) + (w2 * float(c2.b));
                        SDL_SetRenderDrawColor(Camera::image, Uint8(r), Uint8(g), Uint8(b), 255);
                        SDL_RenderDrawPoint(Camera::image, i, j);
                    }
                }
            }
        }

        ImGui::End();
        ImGui::Begin("Ray Casting!");
        ImGui::Checkbox("Render", &raycast);
        if (raycast)
        {
            SDL_SetRenderDrawColor(Camera::image, 0, 0, 0, 0);
            SDL_RenderClear(Camera::image);
            for(int y = 0; y < imageHeight; y++)
            {
                auto rayDirX0 = float(dirX - planeX);
                auto rayDirY0 = float(dirY - planeY);
                auto rayDirX1 = float(dirX + planeX);
                auto rayDirY1 = float(dirY + planeY);
                int p = y - (imageHeight / 2);
                float posZ = 0.5 * imageHeight;
                float rowDistance = posZ / float(p);
                float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / imageWidth;
                float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / imageWidth;
                auto floorX = float(posX) + (rowDistance * rayDirX0);
                auto floorY = float(posY) + (rowDistance * rayDirY0);
                for (int x = 0; x < imageWidth; x++)
                {
                    int cellX = int(floorX);
                    int cellY = int(floorY);
                    int tx = int(TEX_WIDTH * (floorX - float(cellX))) & (TEX_WIDTH - 1);
                    int ty = int(TEX_HEIGHT * (floorY - float(cellY))) & (TEX_HEIGHT - 1);
                    floorX += floorStepX;
                    floorY += floorStepY;
                    int floorTexture = 3;
                    int ceilingTexture = 6;
                    Uint32 uColor;
                    ptr = (Uint8 *)texture[floorTexture]->pixels + (ty * texture[floorTexture]->pitch) + (tx * 3);
                    uColor = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
                    uColor = (uColor >> 1) & 8355711;
                    buffer[(imageWidth * y) + x] = uColor;
                    ptr = (Uint8 *)texture[ceilingTexture]->pixels + (ty * texture[ceilingTexture]->pitch) + (tx * 3);
                    uColor = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
                    uColor = (uColor >> 1) & 8355711;
                    buffer[imageWidth * (imageHeight - y - 1) + x] = uColor;
                }
            }

            for (int x = 0; x < imageWidth; x++)
            {
                double cameraX = (2 * x / double(imageWidth)) - 1;
                double rayDirX = dirX + (planeX * cameraX);
                double rayDirY = dirY + (planeY * cameraX);
                auto mapX = int(posX);
                auto mapY = int(posY);
                double sideDistX;
                double sideDistY;
                double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
                double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
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

                auto lineHeight = int(imageHeight / perpWallDist);
                int drawStart = (-lineHeight / 2) + (imageHeight / 2);
                if (drawStart < 0)
                {
                    drawStart = 0;
                }

                int drawEnd = (lineHeight / 2) + (imageHeight / 2);
                if (drawEnd >= imageHeight)
                {
                    drawEnd = imageHeight - 1;
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
                auto texX = int(wallX * double(TEX_WIDTH));
                if (side == 0 && rayDirX > 0)
                {
                    texX = TEX_WIDTH - texX - 1;
                }

                if (side == 1 && rayDirY < 0)
                {
                    texX = TEX_WIDTH - texX - 1;
                }

                double step = 1.0 * TEX_HEIGHT / lineHeight;
                double texPos = (drawStart - (imageHeight / 2.) + (lineHeight / 2.)) * step;
                for (int y = drawStart; y < drawEnd; y++)
                {
                    auto texY = int(texPos) & (TEX_HEIGHT - 1);
                    texPos += step;
                    ptr = (Uint8 *)texture[texNum]->pixels + (texY * texture[texNum]->pitch) + (texX * 3);
                    Uint32 uColor = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
                    if (side == 1)
                    {
                        uColor = (uColor >> 1) & 8355711;
                    }

                    buffer[(imageWidth * y) + x] = uColor;
                }

                zBuffer[x] = perpWallDist;
            }

            for (int i = 0; i < NUM_SPRITES; i++)
            {
                spriteOrder[i] = i;
                spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); //sqrt not taken, unneeded
            }

            sortSprites(spriteOrder, spriteDistance, NUM_SPRITES);

            for (int i = 0; i < NUM_SPRITES; i++)
            {
                double spriteX = sprite[spriteOrder[i]].x - posX;
                double spriteY = sprite[spriteOrder[i]].y - posY;
                double invDet = 1.0 / ((planeX * dirY) - (dirX * planeY));
                double transformX = invDet * ((dirY * spriteX) - (dirX * spriteY));
                double transformY = invDet * ((-planeY * spriteX) + (planeX * spriteY));
                auto spriteScreenX = int((imageWidth / 2.) * (1 + (transformX / transformY)));
                auto vMoveScreen = int(V_MOVE / transformY);
                int spriteHeight = abs(int(imageHeight / (transformY))) / V_DIV;
                int drawStartY = (-spriteHeight / 2) + (imageHeight / 2) + vMoveScreen;
                if (drawStartY < 0)
                {
                    drawStartY = 0;
                }

                int drawEndY = (spriteHeight / 2) + (imageHeight / 2) + vMoveScreen;
                if (drawEndY >= imageHeight)
                {
                    drawEndY = imageHeight - 1;
                }

                int spriteWidth = abs(int(imageHeight / (transformY))) / U_DIV;
                int drawStartX = (-spriteWidth / 2) + spriteScreenX;
                if (drawStartX < 0)
                {
                    drawStartX = 0;
                }

                int drawEndX = (spriteWidth / 2) + spriteScreenX;
                if (drawEndX >= imageWidth)
                {
                    drawEndX = imageWidth - 1;
                }

                for (int stripe = drawStartX; stripe < drawEndX; stripe++)
                {
                    auto texX = int(256 * (stripe - ((-spriteWidth / 2) + spriteScreenX)) * TEX_WIDTH / spriteWidth) / 256;
                    if (transformY > 0 && stripe > 0 && stripe < imageWidth && transformY < zBuffer[stripe])
                        for (int y = drawStartY; y < drawEndY; y++)
                        {
                            int d = ((y - vMoveScreen) * 256) - (imageHeight * 128) + (spriteHeight * 128);
                            int texY = ((d * TEX_HEIGHT) / spriteHeight) / 256;
                            ptr = (Uint8 *)texture[sprite[spriteOrder[i]].texture]->pixels + (texY * texture[sprite[spriteOrder[i]].texture]->pitch) + (texX * 3);
                            Uint32 uColor = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
                            if ((uColor & 0x00FFFFFF) != 0)
                            {
                                buffer[(imageWidth * y) + stripe] = rgbToInt(((SDL_Color(255, 255, 255, 255) - intToRgb(buffer[(imageWidth * y) + stripe])) / 2) + (intToRgb(uColor) / 2));
                            }
                        }
                }
            }

            SDL_UpdateTexture(frame, nullptr, srf->pixels, srf->pitch);
            SDL_RenderCopy(Camera::image, frame, nullptr, nullptr);
            for (int y = 0; y < imageHeight; y++)
            {
                for (int x = 0; x < imageWidth; x++)
                {
                    buffer[(imageWidth * y) + x] = 0;
                }
            }

            oldTime = time;
            time = double(SDL_GetTicks64());
            double frameTime = (time - oldTime) / 1000.0;
            SDL_SetRenderDrawColor(Camera::image, 255, 255, 255, 255);
            SDLTest_DrawString(Camera::image, 0, 0, std::to_string(1.0 / frameTime).c_str());
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
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Camera::image);
        SDL_RenderPresent(Camera::image);
    }

    SDL_DestroyRenderer(Camera::image);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
