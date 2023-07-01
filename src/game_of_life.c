#include <stdio.h>
#include <stdlib.h>

#define configSize 10  // максимум координат в конфигах - 5.

void inputIntMatrix(int ***matrix);  // заполнение логической матрицы нулями
void outputIntMatrix(int **matrix);  // отрисовка матрицы логики игры (отладка)
void outputCharMatrix(char **matrix);                  // рендер матрицы игры
void freeCharMatrix(char ***matrix);                   // чистка char-матрицы
void inputCharMatrix(char ***matrix);                  // заполнение матрицы точками
void modeHandler(char *mode, int *config);             // обработчик конфигов
void conf1(int *config);                               // первый конфиг
void loadConfig(char **guiM, int **boolM, int *conf);  // подгрузка конфига
void freeIntMatrix(int ***matrix);                     // чистка int-матрицы

enum Sizes { WIDTH = 80, HEIGTH = 25 };

int main() {
    char mode;
    char **guiMatrix = malloc(sizeof(char *) * HEIGTH);
    int **boolMatrix = calloc(HEIGTH, sizeof(int *));
    int *config = malloc(sizeof(int) * configSize);
    modeHandler(&mode, config);   // выбираем номер конфига
    inputIntMatrix(&boolMatrix);  // заполняем логическую матрицу
    inputCharMatrix(&guiMatrix);
    loadConfig(guiMatrix, boolMatrix, config);  // подгружаем конфиг
    // outputIntMatrix(boolMatrix);
    outputCharMatrix(guiMatrix);
    freeCharMatrix(&guiMatrix);
    freeIntMatrix(&boolMatrix);
    free(config);
}

void modeHandler(char *mode, int *config) {
    printf("Choose number of config: ");
    scanf("%c", mode);
    switch (*mode) {
        case '1':
            conf1(config);
            break;
        case '2':
        case '3':
        case '4':
        case '5':
        default:
            printf("Error: no such number of config. Enter again.\n");
            modeHandler(mode, config);
    }
}

void inputIntMatrix(int ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        (*matrix)[row] = calloc(WIDTH, sizeof(int));
    }
}

void outputIntMatrix(int **matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        for (short int col = 0; col < WIDTH; col++) {
            if (col != WIDTH - 1) {
                printf("%d ", matrix[row][col]);
            } else {
                printf("%d ", matrix[row][col]);
            }
        }
        if (row != HEIGTH - 1) {
            putchar('\n');
        }
    }
}

void inputCharMatrix(char ***matrix) {
    *matrix = (char **)malloc(sizeof(char *) * HEIGTH);
    for (short int row = 0; row < HEIGTH; row++) {
        (*matrix)[row] = (char *)malloc(sizeof(char) * WIDTH);
        for (short int col = 0; col < WIDTH; col++) {
            (*matrix)[row][col] = '.';
        }
    }
}

void freeIntMatrix(int ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        free((*matrix)[row]);
    }
    free(*matrix);
}

void freeCharMatrix(char ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        free((*matrix)[row]);
    }
    free(*matrix);
}

void loadConfig(char **guiM, int **boolM, int *conf) {
    for (int i = 0; i < configSize; i += 2) {
        guiM[conf[i]][conf[i + 1]] = '*';
        boolM[conf[i]][conf[i + 1]] = 1;
        // printf("%d %d\n", conf[i], conf[i + 1]);
    }
}

void outputCharMatrix(char **matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        for (short int col = 0; col < WIDTH; col++) {
            if (col != WIDTH - 1) {
                printf("%c ", matrix[row][col]);
            } else {
                printf("%c ", matrix[row][col]);
            }
        }
        if (row != HEIGTH - 1) {
            putchar('\n');
        }
    }
}

void conf1(int *config) {
    config[0] = 1;
    config[1] = 2;
    config[2] = 1;
    config[3] = 5;
    config[4] = 1;
    config[5] = 3;
    config[6] = 1;
    config[7] = 4;
    config[8] = 2;
    config[9] = 5;
}