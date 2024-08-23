#include "Matrix.h"
#include "Vec3.h"

Matrix::Matrix(Vec3f v) : m(4, std::vector<float>(1, 1.f)), rows(4), cols(1)
{
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
}

Matrix Matrix::identity(int dimensions)
{
    Matrix e{dimensions, dimensions};
    for (int i = 0; i < dimensions; i++)
    {
        for (int j = 0; j < dimensions; j++)
        {
            e[i][j] = (i == j ? 1.f : 0.f);
        }
    }

    return e;
}

Matrix Matrix::operator*(const Matrix &a)
{
    Matrix result(rows, a.cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < a.cols; j++)
        {
            result.m[i][j] = 0.f;
            for (int k = 0; k < cols; k++)
            {
                result.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
    }

    return result;
}