#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char *filename) : verts(), faces(), norms()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
    {
        return;
    }

    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            iss >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            Vec3f n;
            iss >> n.x >> n.y >> n.z;
            norms.push_back(n);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp.x >> trash >> tmp.y >> trash >> tmp.z)
            {
                tmp.x--;
                tmp.y--;
                tmp.z--;
                f.push_back(tmp);
            }

            faces.push_back(f);
        }
    }

    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << " vn# " << norms.size() << std::endl;
}

std::vector<int> Model::face(int idx)
{
    std::vector<int> face;
    for (auto &i : faces[idx])
    {
        face.push_back(i.x);
    }

    return face;
}

Vec3f Model::normal(int iface, int nthvert)
{
    int idx = faces[iface][nthvert].z;
    return norms[idx].normalize();
}