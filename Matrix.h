#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows;
    int cols;
    double **data;  
} Matrix;

typedef enum {
    MAT_OK = 0,
    MAT_ERR_DIM,
    MAT_ERR_NOT_SQUARE,
    MAT_ERR_SIZE_LIMIT,
    MAT_ERR_DIV_ZERO,
    MAT_ERR_SINGULAR,
    MAT_ERR_IO,
    MAT_ERR_FORMAT,
    MAT_ERR_ALLOC
} MatStatus;

Matrix matrix_create(int rows, int cols);  
void matrix_free(Matrix* m);
int matrix_is_square(const Matrix* m);
int matrix_empty(const Matrix* m);
void matrix_visualize(const Matrix* m); 
#endif
