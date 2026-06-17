#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include "Matrix.h"

MatStatus readEncrypted(const char* path, Matrix* out);
MatStatus writeEncrypted(const char* path, const Matrix* m);

#endif
