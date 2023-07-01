
#define configSize 10  // максимум координат в конфигах - 5.
#define OPTIONS "Enter option:\n1 - Start\n2 - Settings\n3 - Exit"
#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// field setting
#define WIDTH 80
#define HEIGTH 25

// life's rules
#define NEED_TO_SURVIVE 2
#define NEED_TO_BURN 3
#define MAX_ITERATIONS 10000
#define SLEEP_COUNT 1

// drawing
#define CELL_FULL 35    // char -> #
#define CELL_EMPTY ' '  // char -> ·

#endif

void printBorderY() { printf("-"); }
void printBorderX() { printf("|"); }
void printEmpty() { printf(" "); }
void printGameSymbol() { printf("•"); }
void printDecor() { printf("*"); }
void clearScreen() { printf("\33[0d\33[2J"); }
short int lettersRander(const int row, const int col);
short int decorSymbol(const int row, const int col);

void menu_rander() {
    for (int i = 0; i < HEIGTH; i++) {
        if (i == 0 || i == HEIGTH - 1) {
            for (int j = 0; j < WIDTH; j++) {
                printBorderY();
            }
            continue;
        }
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0 || j == WIDTH - 1) {
                printBorderX();
            } else if (lettersRander(i, j) == 1) {
                printGameSymbol();
            } else if (decorSymbol(i, j) == 1) {
                printBorderX();
            } else if (decorSymbol(i, j) == 2) {
                printDecor();
            } else {
                printEmpty();
            }
        }
        putchar('\n');
    }
    putchar('\n');
    printf("%s\n", OPTIONS);
}

short int lettersRander(const int row, const int col) {
    short int flag = 0;
    if (row == 5 && (col >= 15 && col <= 20)) {
        flag = 1;
    } else if (col == 15 && (row >= 6 && row <= 10)) {
        flag = 1;
    } else if (row == 10 && (col >= 15 && col <= 20)) {
        flag = 1;
    } else if (col == 20 && (row >= 8 && row < 10)) {
        flag = 1;
    } else if (row == 8 && col == 19) {
        flag = 1;
    } else if (row == 5 && (col >= 25 && col <= 30)) {
        flag = 1;
    } else if ((col == 25 || col == 30) && (row >= 6 && row <= 10)) {
        flag = 1;
    } else if (row == 10 && (col > 25 && col < 30)) {
        flag = 1;
    }
    return flag;
}

short int decorSymbol(const int row, const int col) {
    short int flag = 0;
    if (col == 35 && (row >= 5 && row < 9)) {
        flag = 1;
    } else if (col == 35 && row == 10) {
        flag = 2;
    }
    return flag;
}
