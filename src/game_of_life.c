#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config1.c"
#include "gameConfig.h"
#include "menu.c"

void startGame();                    // старт игры
void option_handler();               // обработчик меню
void inputIntMatrix(int ***matrix);  // заполнение логической матрицы нулями
void outputIntMatrix(int **matrix);  // отрисовка матрицы логики игры (отладка)
void outputCharMatrix(char **matrix);                  // рендер матрицы игры
void freeCharMatrix(char ***matrix);                   // чистка char-матрицы
void inputCharMatrix(char ***matrix);                  // заполнение матрицы точками
void confHandler(char *confNumber, int **boolMatrix);  // обработчик конфигов
void conf1(int **lifes);                               // первый конфиг
// void loadConfig(char **guiM, int **boolM, int *conf);  // подгрузка конфига
void freeIntMatrix(int ***matrix);               // чистка int-матрицы
void updateWeights(int **weights, int **lifes);  // обновление граф. матрицы
void updateLifes(int **weights, int **lifes);    // обновление булевой матрицы
int is_there_life_here(int **lifes);             // проверка на жизнь
void draw(int **life_field);                     // отрисовка фигуры
void setZeros(int **matrix);

int main() { option_handler(); }

void option_handler() {
    int option;
    menu_rander();
    scanf("%d", &option);
    switch (option) {
        case 1:
            startGame();
            break;
        case 2:
        case 3:
        default:
            printf("Choose the correct option.\n");
            option_handler();
    }
}

void startGame() {
    char confNumber;    // номер конфига
    int **guiMatrix = malloc(sizeof(int *) * HEIGTH);   // gui-матрица
    int **boolMatrix = calloc(HEIGTH, sizeof(int *));   // bool-матрица
    inputIntMatrix(&boolMatrix);           // заполняем логическую матрицу
    confHandler(&confNumber, boolMatrix);  // выбираем номер конфига
    inputIntMatrix(&guiMatrix);
    // outputIntMatrix(boolMatrix);
    // main loop
    int iter = 0;
    while (is_there_life_here(guiMatrix) &&
           iter < MAX_ITERATIONS) {  // пока есть клетки или счетчик итераций больше чем число
        // основное тело цикла
        // norm_draw(weights);
        draw(guiMatrix);
        printf("\n\n");

        setZeros(boolMatrix);
        updateWeights(boolMatrix, guiMatrix);
        updateLifes(boolMatrix, guiMatrix);

        ++iter;
        sleep(SLEEP_COUNT);
    }

    draw(guiMatrix);
    // loadConfig(guiMatrix, boolMatrix, config);  // подгружаем конфиг
    // outputIntMatrix(boolMatrix);
    // outputCharMatrix(guiMatrix);
    freeIntMatrix(&guiMatrix);
    freeIntMatrix(&boolMatrix);
}

void confHandler(char *confNumber, int **boolMatrix) {
    printf("Choose number of config: ");
    scanf("%c", confNumber);
    switch (*confNumber) {
        case '1':
            conf1(boolMatrix);
            break;
        case '2':
        case '3':
        case '4':
        case '5':
        default:
            printf("Error: no such number of config. Enter again.\n");
            confHandler(confNumber, boolMatrix);
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
            (*matrix)[row][col] = ' ';
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

void updateWeights(int **weights, int **lifes) {
    for (int i = 0; i < HEIGTH; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (lifes[i][j] == 1) {                 // cell is full -> update weights neigbors
                weights[(i + 1) % HEIGTH][j] += 1;  // вниз
                // norm_draw(weights);
                weights[i][(j + 1) % WIDTH] += 1;  // вправо
                // norm_draw(weights);
                weights[(i + 1) % HEIGTH][(j + 1) % WIDTH] += 1;  // вниз и вправо
                // norm_draw(weights);

                if (i == 0) {
                    weights[HEIGTH - 1][(j + 1) % WIDTH] += 1;  // вверх и вправо на границе
                    // norm_draw(weights);
                    weights[HEIGTH - 1][j] += 1;  // вверх на границе
                    // norm_draw(weights);
                } else {
                    weights[i - 1][(j + 1) % WIDTH] += 1;  // вверх и вправо в поле
                    // norm_draw(weights);
                    weights[i - 1][j] += 1;  // вверх в поле
                    // norm_draw(weights);
                }

                if (i == 0 && j == 0)
                    weights[HEIGTH - 1][WIDTH - 1] += 1;  // влево вверх если в самом углу
                else if (i == 0)
                    weights[HEIGTH - 1][j - 1] += 1;  // влево вверх если на границе сверху
                else if (j == 0)
                    weights[i - 1][WIDTH - 1] += 1;  // влево вверх если на границе слева
                else
                    weights[i - 1][j - 1] += 1;  // влево вверх если на поле
                // norm_draw(weights);

                if (j == 0) {
                    weights[i][WIDTH - 1] += 1;  // влево если на границе
                    // norm_draw(weights);
                    weights[(i + 1) % HEIGTH][WIDTH - 1] += 1;  // влево вниз если на границе
                    // norm_draw(weights);
                } else {
                    weights[i][j - 1] += 1;  // влево если в поле
                    // norm_draw(weights);
                    weights[(i + 1) % HEIGTH][j - 1] += 1;  // влево вниз если на границе
                    // norm_draw(weights);
                }
            }
        }
    }
}

void updateLifes(int **weights, int **lifes) {
    for (int i = 0; i < HEIGTH; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (lifes[i][j] == 0 && weights[i][j] >= NEED_TO_BURN)
                lifes[i][j] = 1;  // клетка родилась, т.к. соседей больше NEED_TO_BURN
            if (lifes[i][j] == 1) {
                // если клетка уже существовала
                if (weights[i][j] >= NEED_TO_SURVIVE && weights[i][j] <= NEED_TO_SURVIVE + 1)
                    lifes[i][j] = 1;  // клетка умирает
                else
                    lifes[i][j] = 0;
            }
        }
    }
}

int is_there_life_here(int **lifes) {
    int flag = 0;
    for (int i = 0; i < HEIGTH; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (lifes[i][j]) flag = 1;
        }
    }
    return flag;
}

void draw(int **life_field) {
    char current_char = '!';
    for (int i = 0; i < HEIGTH; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            current_char = (life_field[i][j] == 1) ? CELL_FULL : CELL_EMPTY;
            if (j == WIDTH - 1)
                printf("%c ", current_char);
            else
                printf("%c", current_char);
        }
        if (i != HEIGTH - 1) printf("\n");
    }
}

void setZeros(int **matrix) {
    for (int i = 0; i < HEIGTH; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            matrix[i][j] = 0;
        }
    }
}