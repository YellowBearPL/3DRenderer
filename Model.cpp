#include "Model.h"

std::vector<int> Model::face(int idx)
{
    std::vector<int> face;
    for (auto &i : faces[idx])
    {
        face.push_back(i.x);
    }

    return face;
}