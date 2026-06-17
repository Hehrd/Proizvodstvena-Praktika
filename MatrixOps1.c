#include "MatrixOps1.h"

MatStatus multiplyByScalar(const Matrix* m, double scalar, Matrix* out) {
    *out = matrix_create(m->rows, m->cols);
    if (matrix_empty(out)) return MAT_ERR_ALLOC;
    for (int i = 0; i < m->rows; ++i)
        for (int j = 0; j < m->cols; ++j)
            out->data[i][j] = m->data[i][j] * scalar;
    return MAT_OK;
}

MatStatus matrixMultiply(const Matrix* a, const Matrix* b, Matrix* out) {
    if (a->cols != b->rows) return MAT_ERR_DIM;
    *out = matrix_create(a->rows, b->cols);
    if (matrix_empty(out)) return MAT_ERR_ALLOC;
    for (int i = 0; i < a->rows; ++i)
        for (int j = 0; j < b->cols; ++j) {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k)
                sum += a->data[i][k] * b->data[k][j];
            out->data[i][j] = sum;
        }
    return MAT_OK;
}

static Matrix minor_matrix(const Matrix* m, int er, int ec) {
    Matrix r = matrix_create(m->rows - 1, m->cols - 1);
    if (matrix_empty(&r)) return r;
    int rr = 0;
    for (int i = 0; i < m->rows; ++i) {
        if (i == er) continue;
        int cc = 0;
        for (int j = 0; j < m->cols; ++j) {
            if (j == ec) continue;
            r.data[rr][cc] = m->data[i][j];
            ++cc;
        }
        ++rr;
    }
    return r;
}

static double det_recursive(const Matrix* m) {
    int n = m->rows;
    if (n == 1) return m->data[0][0];
    if (n == 2)
        return m->data[0][0] * m->data[1][1] - m->data[0][1] * m->data[1][0];

    double det = 0.0;
    int sign = 1;
    for (int j = 0; j < n; ++j) {
        Matrix sub = minor_matrix(m, 0, j);
        det += sign * m->data[0][j] * det_recursive(&sub);
        matrix_free(&sub);
        sign = -sign;
    }
    return det;
}

MatStatus determinant(const Matrix* m, double* out) {
    if (!matrix_is_square(m)) return MAT_ERR_NOT_SQUARE;
    if (m->rows > 4)          return MAT_ERR_SIZE_LIMIT;
    *out = det_recursive(m);
    return MAT_OK;
}
