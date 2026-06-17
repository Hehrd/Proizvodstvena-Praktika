#ifndef MATRIX_OPS2_H
#define MATRIX_OPS2_H

#include "Matrix.h"

MatStatus inverseMat(const Matrix* m, Matrix* out);
MatStatus divideByScalar(const Matrix* m, double scalar, Matrix* out);
MatStatus transposeMat(const Matrix* m, Matrix* out);

#endif /* MATRIX_OPS2_H */
