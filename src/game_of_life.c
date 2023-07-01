#include <stdio.h>
#include <stdlib.h>

void input(int ***matrix);  // заполнение поля нулями
void output(int **matrix);  // вывод поля
void free_matrix(int ***matrix);


enum Sizes {
    WIDTH = 80,
    HEIGTH = 25
};


int main() {
    // printf("%d", WIDTH);
    int **field = calloc(HEIGTH, sizeof(int*));
    input(&field);
    output(field);
    free_matrix(&field);
   
}

void input(int ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        (*matrix)[row] = calloc(WIDTH, sizeof(int));
    }
}


void output(int **matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        for (short int col = 0; col < WIDTH; col++) {
            printf("%d ", matrix[row][col]);
        }
        putchar('\n');
    }
}

void free_matrix(int ***matrix) {
    for (short int row = 0; row < HEIGTH; row++) {
        free((*matrix)[row]);
    }
    free(*matrix);
}