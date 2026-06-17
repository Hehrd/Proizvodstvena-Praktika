#include "MatrixIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char CRYPTO_KEY[] = "primer!";

static void xor_crypt(char* buf, long n) {
    long klen = (long)strlen(CRYPTO_KEY);
    for (long i = 0; i < n; ++i)
        buf[i] = (char)(buf[i] ^ CRYPTO_KEY[i % klen]);
}

MatStatus writeEncrypted(const char* path, const Matrix* m) {
    long cap = (long)m->rows * m->cols * 48 + 64;
    char* buf = (char*)malloc((size_t)cap);
    if (!buf) return MAT_ERR_ALLOC;

    long len = 0;
    len += sprintf(buf + len, "%d %d\n", m->rows, m->cols);
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            len += sprintf(buf + len, "%g", m->data[i][j]);
            if (j + 1 < m->cols) buf[len++] = ' ';
        }
        buf[len++] = '\n';
    }

    xor_crypt(buf, len);

    FILE* f = fopen(path, "wb");
    if (!f) { free(buf); return MAT_ERR_IO; }
    fwrite(buf, 1, (size_t)len, f);
    fclose(f);
    free(buf);
    return MAT_OK;
}

MatStatus readEncrypted(const char* path, Matrix* out) {
    FILE* f = fopen(path, "rb");
    if (!f) return MAT_ERR_IO;

    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) { fclose(f); return MAT_ERR_IO; }

    char* buf = (char*)malloc((size_t)n + 1);
    if (!buf) { fclose(f); return MAT_ERR_ALLOC; }

    size_t got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[got] = '\0';

    xor_crypt(buf, (long)got);  

    int rows, cols, consumed = 0;
    if (sscanf(buf, "%d %d%n", &rows, &cols, &consumed) != 2 ||
        rows <= 0 || cols <= 0) {
        free(buf);
        return MAT_ERR_FORMAT;
    }

    Matrix m = matrix_create(rows, cols);
    if (matrix_empty(&m)) { free(buf); return MAT_ERR_ALLOC; }

    char* p = buf + consumed;
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < cols; ++j){
            char* end;
            double v = strtod(p, &end);
            if (end == p) {               
                matrix_free(&m);
                free(buf);
                return MAT_ERR_FORMAT;
            }
            m.data[i][j] = v;
            p = end;
        }
    }
    free(buf);
    *out = m;
    return MAT_OK;
}
