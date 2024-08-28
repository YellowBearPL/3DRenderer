#ifndef INC_3DRENDERER_MODEL_H
#define INC_3DRENDERER_MODEL_H
#include "Geometry.h"
#include <string>
#include <vector>

class Model
{
private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<Vec3i>> faces;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> vUv;
    SDL_Surface *diffusemap;
    SDL_Surface *normalmap;
    SDL_Surface *specularmap;

    static void loadTexture(std::string filename, std::string suffix, SDL_Surface *&img);

public:
    explicit Model(const char *filename);

    int nfaces() { return int(faces.size()); }

    Vec3f normal(int iface, int nthvert);

    Vec3f normal(const Vec2f &uvf);

    std::vector<int> face(int idx);

    Vec3f vert(int i) { return verts[i]; }

    Vec3f vert(int iface, int nthvert) { return verts[faces[iface][nthvert].x]; }

    Vec2f uv(int iface, int nthvert) { return vUv[faces[iface][nthvert].y]; };
};
#endif//INC_3DRENDERER_MODEL_H