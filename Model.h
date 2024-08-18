#ifndef INC_3DRENDERER_MODEL_H
#define INC_3DRENDERER_MODEL_H
#include "Vec3.h"
#include <vector>

class Model
{
private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<int>> faces;

public:
    explicit Model(const char *filename);
    int nfaces() { return int(faces.size()); }
    std::vector<int> face(int idx) { return faces[idx]; }
    Vec3f vert(int i) { return verts[i]; }
};
#endif//INC_3DRENDERER_MODEL_H