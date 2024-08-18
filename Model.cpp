#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char *filename) : verts(), faces()
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
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                idx--;
                f.push_back(idx);
            }

            faces.push_back(f);
        }
    }

    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << std::endl;
}