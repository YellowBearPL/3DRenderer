#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

Model::Model(const char *filename) : verts(), faces(), norms(), vUv(), diffusemap()
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
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            Vec2f v2Uv;
            iss >> v2Uv.u >> v2Uv.v;
            vUv.push_back(v2Uv);
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

    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << " vt# " << vUv.size() << " vn# " << norms.size() << std::endl;
    loadTexture(filename, "_diffuse.bmp", diffusemap);
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

void Model::loadTexture(std::string filename, std::string suffix, SDL_Surface *&img)
{
    std::string texfile{std::move(filename)};
    size_t dot = texfile.find_last_of('.');
    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(std::move(suffix));
        std::cerr << "texture file " << texfile << " loading " << ((img = SDL_LoadBMP(texfile.c_str())) ? "ok" : "failed") << std::endl;
    }
}

SDL_Color Model::diffuse(Vec2f uvf)
{
    Vec2i uv{static_cast<int>(fmod(abs(uvf.u), 1) * diffusemap->w), static_cast<int>(fmod(abs(uvf.v), 1) * diffusemap->h)};
    auto *p = (Uint8 *)diffusemap->pixels + (uv.v * diffusemap->pitch) + (uv.u * 3);
    return {p[2], p[1], p[0], 255};
}