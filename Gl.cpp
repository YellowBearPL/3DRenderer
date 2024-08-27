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

float Shader::depth = 2000.f;

void Shader::triangle(std::vector<Vec4f> pts, SDL_Renderer *image, std::vector<float> &zbuffer)
{
    Vec2f bboxmin{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vec2f bboxmax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
    for (int i = 0; i < 3; i++)
    {
        bboxmin.u = std::min(bboxmin.u, pts[i][0] / pts[i][3]);
        bboxmax.u = std::max(bboxmax.u, pts[i][0] / pts[i][3]);
        bboxmin.v = std::min(bboxmin.v, pts[i][1] / pts[i][3]);
        bboxmax.v = std::max(bboxmax.v, pts[i][1] / pts[i][3]);
    }

    Vec2i p;
    for (p.u = int(bboxmin.u); p.u < width && float(p.u) <= bboxmax.u; p.u++)
    {
        for (p.v = int(bboxmin.v); p.v < height && float(p.v) <= bboxmax.v; p.v++)
        {
            Vec3f c = (pts[0] / pts[0][3]).proj2().barycentric((pts[1] / pts[1][3]).proj2(), (pts[2] / pts[2][3]).proj2(), p.proj2());
            float z = (pts[0][2] * c.x) + (pts[1][2] * c.y) + (pts[2][2] * c.z);
            float w = (pts[0][3] * c.x) + (pts[1][3] * c.y) + (pts[2][3] * c.z);
            int fragDepth = std::max(long(0), std::min(long(255), std::lround(z / w)));
            if (c.x < 0 || c.y < 0 || c.z < 0 || zbuffer[(p.v * width) + p.u] > float(fragDepth))
            {
                continue;
            }

            SDL_Color color;
            bool discard = fragment(c, color);
            if (!discard)
            {
                zbuffer[(p.v * width) + p.u] = float(fragDepth);
                SDL_SetRenderDrawColor(image, color.r, color.g, color.b, color.a);
                SDL_RenderDrawPoint(image, p.u, p.v);
            }
        }
    }
}