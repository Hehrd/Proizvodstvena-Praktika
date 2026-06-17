#include "Matrix.h"
#include "MatrixOps1.h"
#include "MatrixOps2.h"
#include "MatrixIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MATRICES 64
#define NAME_LEN     64
#define LINE_LEN     256

typedef struct {
    char   name[NAME_LEN];
    Matrix m;
    int    used;
} Entry;

static Entry storage[MAX_MATRICES];

static Entry* find_entry(const char* name) {
    for (int i = 0; i < MAX_MATRICES; ++i)
        if (storage[i].used && strcmp(storage[i].name, name) == 0)
            return &storage[i];
    return NULL;
}

static Entry* store_matrix(const char* name, Matrix m) {
    Entry* e = find_entry(name);
    if (e) {
        matrix_free(&e->m);
        e->m = m;
        return e;
    }
    for (int i = 0; i < MAX_MATRICES; ++i)
        if (!storage[i].used) {
            storage[i].used = 1;
            strncpy(storage[i].name, name, NAME_LEN - 1);
            storage[i].name[NAME_LEN - 1] = '\0';
            storage[i].m = m;
            return &storage[i];
        }
    return NULL;
}

static void list_matrices(void) {
    int any = 0;
    printf("Налични матрици в паметта: ");
    for (int i = 0; i < MAX_MATRICES; ++i)
        if (storage[i].used) {
            printf("%s%s (%dx%d)", any ? ", " : "",
                   storage[i].name, storage[i].m.rows, storage[i].m.cols);
            any = 1;
        }
    printf(any ? "\n" : "(няма)\n");
}


static void read_line(const char* prompt, char* buf, int size) {
    printf("%s", prompt);
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    size_t n = strlen(buf);
    if (n > 0 && buf[n - 1] == '\n') buf[n - 1] = '\0';
}

static int read_int(const char* prompt) {
    char buf[LINE_LEN];
    for (;;) {
        read_line(prompt, buf, sizeof(buf));
        char* end;
        long v = strtol(buf, &end, 10);
        if (end != buf) return (int)v;
        printf("Невалидно цяло число, опитайте отново.\n");
    }
}

static double read_double(const char* prompt) {
    char buf[LINE_LEN];
    for (;;) {
        read_line(prompt, buf, sizeof(buf));
        char* end;
        double v = strtod(buf, &end);
        if (end != buf) return v;
        printf("Невалидно число, опитайте отново.\n");
    }
}

static Entry* ask_matrix(const char* prompt) {
    char name[NAME_LEN];
    read_line(prompt, name, sizeof(name));
    Entry* e = find_entry(name);
    if (!e) printf("Няма матрица с име '%s' в паметта.\n", name);
    return e;
}

static void offer_save(Matrix result) {
    char alias[NAME_LEN];
    int stored = 0;

    read_line("Запази резултата в паметта под име (празно = пропусни): ",
              alias, sizeof(alias));
    if (alias[0] != '\0') {
        if (store_matrix(alias, result)) {
            printf("Запазено в паметта като '%s'.\n", alias);
            stored = 1;
        } else {
            printf("Паметта е пълна.\n");
        }
    }

    char path[LINE_LEN];
    read_line("Запиши във криптиран файл - въведи път (празно = пропусни): ",
              path, sizeof(path));
    if (path[0] != '\0') {
        if (writeEncrypted(path, &result) == MAT_OK)
            printf("Записано във '%s'.\n", path);
        else
            printf("Грешка при запис във файла.\n");
    }

    if (!stored) matrix_free(&result);
}

static void load_from_file(void) {
    char path[LINE_LEN], alias[NAME_LEN];
    read_line("Път към криптиран файл: ", path, sizeof(path));
    read_line("Име (alias) на матрицата: ", alias, sizeof(alias));
    if (alias[0] == '\0') {
        printf("Името не може да е празно.\n");
        return;
    }
    Matrix m;
    if (readEncrypted(path, &m) != MAT_OK) {
        printf("Грешка при четене на файла (липсва файл или невалиден формат).\n");
        return;
    }
    if (!store_matrix(alias, m)) {
        printf("Паметта е пълна.\n");
        matrix_free(&m);
        return;
    }
    printf("Матрицата е заредена като '%s'.\n", alias);
    matrix_visualize(&m);
}

static void perform_operation(void) {
    printf("\n--- Операции ---\n"
           "1. Умножение на матрица със скалар\n"
           "2. Умножение на матрица с матрица\n"
           "3. Детерминанта (до 4x4)\n"
           "4. Деление на матрица по скалар\n"
           "5. Обратна матрица\n"
           "6. Транспониране\n");
    int op = read_int("Избор: ");
    list_matrices();

    Matrix out;
    Entry *a, *b;
    double s;

    switch (op) {
        case 1:
            a = ask_matrix("Име на матрица: ");
            if (!a) return;
            s = read_double("Скалар: ");
            if (multiplyByScalar(&a->m, s, &out) != MAT_OK) {
                printf("Грешка при умножението (недостатъчна памет).\n");
                return;
            }
            break;
        case 2:
            a = ask_matrix("Име на първа матрица: ");
            if (!a) return;
            b = ask_matrix("Име на втора матрица: ");
            if (!b) return;
            if (matrixMultiply(&a->m, &b->m, &out) != MAT_OK) {
                printf("Грешка: броят колони на първата трябва да е равен "
                       "на броя редове на втората.\n");
                return;
            }
            break;
        case 3: {
            a = ask_matrix("Име на матрица: ");
            if (!a) return;
            double d;
            if (determinant(&a->m, &d) != MAT_OK) {
                printf("Грешка: детерминанта се намира само за квадратна матрица до 4x4.\n");
                return;
            }
            printf("Детерминанта = %g\n", d);
            return;   /* детерминантата е число, няма какво да се записва */
        }
        case 4:
            a = ask_matrix("Име на матрица: ");
            if (!a) return;
            s = read_double("Скалар: ");
            if (divideByScalar(&a->m, s, &out) != MAT_OK) {
                printf("Грешка: деление на нула не е възможно.\n");
                return;
            }
            break;
        case 5:
            a = ask_matrix("Име на матрица: ");
            if (!a) return;
            if (inverseMat(&a->m, &out) != MAT_OK) {
                printf("Грешка: матрицата трябва да е квадратна (до 4x4) и неизродена.\n");
                return;
            }
            break;
        case 6:
            a = ask_matrix("Име на матрица: ");
            if (!a) return;
            if (transposeMat(&a->m, &out) != MAT_OK) {
                printf("Грешка при транспонирането (недостатъчна памет).\n");
                return;
            }
            break;
        default:
            printf("Невалиден избор.\n");
            return;
    }

    matrix_visualize(&out);
    offer_save(out);
}

static void visualize_matrix(void) {
    list_matrices();
    Entry* a = ask_matrix("Име на матрица за визуализация: ");
    if (a) matrix_visualize(&a->m);
}

static void manual_input(void) {
    int rows = read_int("Брой редове: ");
    int cols = read_int("Брой колони: ");
    Matrix m = matrix_create(rows, cols);
    if (matrix_empty(&m)) {
        printf("Невалидни размери.\n");
        return;
    }

    printf("Въведете елементите ред по ред:\n");
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            char prompt[64];
            sprintf(prompt, "  [%d][%d] = ", i, j);
            m.data[i][j] = read_double(prompt);
        }

    matrix_visualize(&m);

    char alias[NAME_LEN];
    int stored = 0;
    read_line("Запази в паметта под име (празно = пропусни): ", alias, sizeof(alias));
    if (alias[0] != '\0') {
        if (store_matrix(alias, m)) {
            printf("Запазено в паметта като '%s'.\n", alias);
            stored = 1;
        }
    }

    char path[LINE_LEN];
    read_line("Път за записване в криптиран файл: ", path, sizeof(path));
    if (path[0] != '\0') {
        if (writeEncrypted(path, &m) == MAT_OK)
            printf("Записано във '%s'.\n", path);
        else
            printf("Грешка при запис във файла.\n");
    }

    if (!stored) matrix_free(&m);
}

static void cleanup(void) {
    for (int i = 0; i < MAX_MATRICES; ++i)
        if (storage[i].used) matrix_free(&storage[i].m);
}

int main(){
    do{
        printf("\n--- Главно меню ---\n"
               "1. Зареди матрица от паметта (криптиран файл)\n"
               "2. Извърши операция с матрици\n"
               "3. Визуализирай матрица\n"
               "4. Ръчно въвеждане на матрица и запис във файл\n"
               "0. Изход\n");
        int choice = read_int("Избор: ");

        switch (choice) {
            case 1: load_from_file();    break;
            case 2: perform_operation(); break;
            case 3: visualize_matrix();  break;
            case 4: manual_input();      break;
            case 0: cleanup(); printf("Довиждане!\n"); return 0;
            default: printf("Невалиден избор.\n");
        }
    }while(1);
}
