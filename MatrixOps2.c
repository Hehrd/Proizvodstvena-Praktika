#include "MatrixOps2.h"
#include "MatrixOps1.h"
#include <math.h>


MatStatus transposeMat(const Matrix* m, Matrix* out) {
    *out = matrix_create(m->cols, m->rows);
    if (matrix_empty(out)) return MAT_ERR_ALLOC;
    for (int i = 0; i < m->rows; ++i)
        for (int j = 0; j < m->cols; ++j)
            out->data[j][i] = m->data[i][j];
    return MAT_OK;
}

MatStatus divideByScalar(const Matrix* m, double scalar, Matrix* out) {
    if (fabs(scalar) < 1e-12) return MAT_ERR_DIV_ZERO;
    *out = matrix_create(m->rows, m->cols);
    if (matrix_empty(out)) return MAT_ERR_ALLOC;
    for (int i = 0; i < m->rows; ++i)
        for (int j = 0; j < m->cols; ++j)
            out->data[i][j] = m->data[i][j] / scalar;
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

MatStatus inverseMat(const Matrix* m, Matrix* out) {
    if (!matrix_is_square(m)) return MAT_ERR_NOT_SQUARE;

    double det;
    MatStatus st = determinant(m, &det);
    if (st != MAT_OK) return st;
    if (fabs(det) < 1e-12) return MAT_ERR_SINGULAR;

    int n = m->rows;
    if (n == 1) {
        *out = matrix_create(1, 1);
        if (matrix_empty(out)) return MAT_ERR_ALLOC;
        out->data[0][0] = 1.0 / m->data[0][0];
        return MAT_OK;
    }

    Matrix cof = matrix_create(n, n);
    if (matrix_empty(&cof)) return MAT_ERR_ALLOC;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            Matrix sub = minor_matrix(m, i, j);
            double minor_det;
            determinant(&sub, &minor_det);   /* sub е (n-1)x(n-1) <= 3x3 */
            matrix_free(&sub);
            int sign = ((i + j) % 2 == 0) ? 1 : -1;
            cof.data[i][j] = sign * minor_det;
        }

    Matrix adj;
    st = transposeMat(&cof, &adj);
    matrix_free(&cof);
    if (st != MAT_OK) return st;

    st = divideByScalar(&adj, det, out);
    matrix_free(&adj);
    return st;
}
