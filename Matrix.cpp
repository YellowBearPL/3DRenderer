#include "Matrix.h"

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