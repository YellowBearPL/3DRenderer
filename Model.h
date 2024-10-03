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

public:
    std::vector<int> face(int idx);

    Vec3f vert(int i) { return verts[i]; }

    Vec3f vert(int iface, int nthvert) { return verts[faces[iface][nthvert].x]; }
};
#endif//INC_3DRENDERER_MODEL_H