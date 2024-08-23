#ifndef INC_3DRENDERER_MATRIX_H
#define INC_3DRENDERER_MATRIX_H
#include <vector>

class Matrix
{
    std::vector<std::vector<float>> m;
    int rows, cols;

public:
    explicit Matrix(int r = 4, int c = 4) : m(r, std::vector<float>(c, 0.f)), rows(r), cols(c) {}

    static Matrix identity(int dimensions = 4);

    std::vector<float> &operator[](const int i) { return m[i]; }

    Matrix operator*(const Matrix &a);
};
#endif//INC_3DRENDERER_MATRIX_H