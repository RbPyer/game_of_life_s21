#include <stdio.h>
#include <stdlib.h>

void input(int ***matrix);  // заполнение поля нулями
void output(char **matrix);  // вывод поля
void free_matrix(char ***matrix);
void input_matrix(char ***matrix);  // заполнение матрицы точками


enum Sizes {
    WIDTH = 80,
    HEIGTH = 25
};


int main() {
    // printf("%d", WIDTH);
    // int **field = calloc(HEIGTH, sizeof(int*));
    // input(&field);
    // output(field);
    // char **matrix = malloc(sizeof(char*) * HEIGTH);
    char **matrix;
    input_matrix(&matrix);
    output(matrix);
    free_matrix(&matrix);
    // free_matrix(&field);
}

void input(int ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        (*matrix)[row] = calloc(WIDTH, sizeof(int));
    }
}


void output(char **matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        for (short int col = 0; col < WIDTH; col++) {
            printf("%c ", matrix[row][col]);
        }
        putchar('\n');
    }
}

void input_matrix(char ***matrix) {
    *matrix = (char **) malloc(sizeof(char*) * HEIGTH);
    for (short int row = 0; row < HEIGTH; row++) {
        (*matrix)[row] = (char *) malloc(sizeof(char) * WIDTH);
        for (short int col = 0; col < WIDTH; col++) {
            (*matrix)[row][col] = '#';
        }
    }
}


void free_matrix(char ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        free((*matrix)[row]);
    }
    free(*matrix);
}