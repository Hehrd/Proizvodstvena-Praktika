#ifndef MATRIX_OPS1_H
#define MATRIX_OPS1_H

#include "Matrix.h"

MatStatus multiplyByScalar(const Matrix* m, double scalar, Matrix* out);
MatStatus matrixMultiply(const Matrix* a, const Matrix* b, Matrix* out);
MatStatus determinant(const Matrix* m, double* out);

#endif
