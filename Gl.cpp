#include "Gl.h"
#include <cmath>

Matrix modelView;
Matrix mViewport;
Matrix mProjection;

void viewport(int x, int y, int w, int h)
{
    mViewport = Matrix::identity();
    mViewport[0][3] = float(x + w) / 2.f;
    mViewport[1][3] = float(y + h) / 2.f;
    mViewport[2][3] = 255.f / 2.f;
    mViewport[0][0] = float(w) / 2.f;
    mViewport[1][1] = float(h) / 2.f;
    mViewport[2][2] = 255.f / 2.f;
}

void projection(float coeff)
{
    mProjection = Matrix::identity();
    mProjection[3][2] = coeff;
}

void Shader::triangle(Mat43<float> &clipc, SDL_Renderer *image, std::vector<float> &zbuffer)
{
    Mat34<float> pts  = (mViewport * clipc).transpose();
    Mat32<float> pts2;
    for (int i = 0; i < 3; i++)
    {
        pts2[i] = (pts[i] / pts[i][3]).proj2();
    }

    Vec2f bboxmin{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vec2f bboxmax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
    Vec2f clamp{static_cast<float>(screenWidth - 1), static_cast<float>(screenHeight - 1)};
    for (int i = 0; i < 3; i++)
    {
        bboxmin.u = std::max(0.f, std::min(bboxmin.u, pts2[i].u));
        bboxmax.u = std::min(clamp.u, std::max(bboxmax.u, pts2[i].u));
        bboxmin.v = std::max(0.f, std::min(bboxmin.v, pts2[i].v));
        bboxmax.v = std::min(clamp.v, std::max(bboxmax.v, pts2[i].v));
    }

    Vec2i p;
    SDL_Color color;
    for (p.u = int(bboxmin.u); float(p.u) <= bboxmax.u; p.u++)
    {
        for (p.v = int(bboxmin.v); float(p.v) <= bboxmax.v; p.v++)
        {
            Vec3f bcScreen = pts2[0].barycentric(pts2[1], pts2[2], {static_cast<float>(p.u), static_cast<float>(p.v)});
            Vec3f bcClip{bcScreen.x / pts[0][3], bcScreen.y / pts[1][3], bcScreen.z / pts[2][3]};
            bcClip /= bcClip.x + bcClip.y + bcClip.z;
            float fragDepth = clipc[2] * bcClip;
            if (bcScreen.x < 0 || bcScreen.y < 0 || bcScreen.z < 0 || zbuffer[p.u + (p.v * screenWidth)] > fragDepth)
            {
                continue;
            }

            bool discard = fragment({static_cast<float>(p.u), static_cast<float>(p.v), fragDepth}, bcClip, color);
            if (!discard)
            {
                zbuffer[p.u + (p.v * screenWidth)] = fragDepth;
                SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
                SDL_RenderDrawPoint(image, p.u, p.v);
            }
        }
    }
}