#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>

Matrix matrix_create(int rows, int cols) {
    Matrix m;
    m.rows = 0;
    m.cols = 0;
    m.data = NULL;
    if (rows <= 0 || cols <= 0)
        return m;

    m.data = (double**)malloc(sizeof(double*) * rows);
    if (!m.data)
        return m;

    for (int i = 0; i < rows; ++i) {
        m.data[i] = (double*)calloc(cols, sizeof(double));
        if (!m.data[i]) {
            for (int k = 0; k < i; ++k) free(m.data[k]);
            free(m.data);
            m.data = NULL;
            return m;
        }
    }
    m.rows = rows;
    m.cols = cols;
    return m;
}

void matrix_free(Matrix* m) {
    if (!m || !m->data) return;
    for (int i = 0; i < m->rows; ++i) free(m->data[i]);
    free(m->data);
    m->data = NULL;
    m->rows = 0;
    m->cols = 0;
}

int matrix_is_square(const Matrix* m) {
    return m->rows == m->cols && m->rows > 0;
}

int matrix_empty(const Matrix* m) {
    return m->rows == 0 || m->cols == 0 || m->data == NULL;
}

void matrix_visualize(const Matrix* m) {
    if (matrix_empty(m)) {
        printf("(празна матрица)\n");
        return;
    }
    printf("Матрица %dx%d:\n", m->rows, m->cols);
    for (int i = 0; i < m->rows; ++i) {
        printf("[ ");
        for (int j = 0; j < m->cols; ++j)
            printf("%10.4f ", m->data[i][j]);
        printf("]\n");
    }
}
