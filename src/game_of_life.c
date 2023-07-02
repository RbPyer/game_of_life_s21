#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
//######################################################SETTINGS################################################################################

// field setting
#define COLS 80
#define ROWS 25

// life's rules
#define NEED_TO_SURVIVE 2
#define NEED_TO_BURN 3
#define MAX_ITERATIONS 100000

// drawing
#define CELL_FULL '#'   // char -> #
#define CELL_EMPTY '.'  // char -> ·

// file
#define FILE_PATH "configs/"

// options for menu
#define OPTIONS "Enter option:\n1 - Start\n2 - Exit\n\nCommands:\nA - speed down.\nZ - speed up.\nQ - exit.\n"

struct termios term;
//###########################################################FUNCS#############################################################################

// memory
int allocateMatrix(int*** matrix);
void freeMatrix(int*** matrix);

// draw
void draw(int** life_field);

// logic
void updateWeights(int** weights, int** lifes);
void updateLifes(int** weights, int** lifes);
int isThereLifeHere(int** lifes);
void setZeros(int** matrix);

// game
void run();
void termOn();
void termOff();
int readInput();

// file and configs
void fileReader(int* array, const char* file_name);
void loadConfig(int* array, int file_order);
void confHandler(int** lifes);

// menu
void printBorderY();
void printBorderX();
void printEmpty();
void printGameSymbol();
void printDecor();
void clearScreen();
short int lettersRander(const int row, const int col);
short int decorSymbol(const int row, const int col);
void menuRander();
void optionHandler();

// ##########################################################BODY###########################################################################
int main() {
    optionHandler();

    return 0;
}

void run() {
    int **weights, **lifes;
    if (allocateMatrix(&weights) && allocateMatrix(&lifes)) {
        int SLEEP_COUNT = 20000;
        setZeros(weights);
        setZeros(lifes);
        confHandler(lifes);
        termOn();
        int stdin_fd = fileno(stdin);
        fcntl(stdin_fd, F_SETFL, O_NONBLOCK);  // Включение неблокирующего режима чтения
        int iter = 0;
        while (isThereLifeHere(lifes) &&
               iter < MAX_ITERATIONS) {  // пока есть клетки или счетчик итераций больше чем число
            int ch = readInput();
            read(stdin_fd, &ch, sizeof(char));
            if ((ch == 'a' || ch == 'A') && SLEEP_COUNT < 10e5) {
                SLEEP_COUNT += 10000;
            }
            if ((ch == 'z' || ch == 'Z') && SLEEP_COUNT >= 15000) {
                SLEEP_COUNT -= 10000;
            }
            if (ch == 'q' || ch == 'Q') {
                puts("Game over.");
                break;
            }
            draw(lifes);
            setZeros(weights);
            updateWeights(weights, lifes);
            updateLifes(weights, lifes);
            ++iter;
            usleep(SLEEP_COUNT);
            clearScreen();
        }
        termOff();
    } else {
        printf("n/a");
    }
    freeMatrix(&weights);
    freeMatrix(&lifes);
}

void fileReader(int* array, const char* file_name) {
    FILE* file;
    // Открыть файл для чтения
    file = fopen(file_name, "r");
    fseek(file, 0, SEEK_END);
    rewind(file);
    // Выделить память для хранения чисел
    // Считать числа из файла
    int i = 0;
    while (fscanf(file, "%d", &array[i]) != EOF) {
        i++;
    }
    // Освободить память и закрыть файл
    fclose(file);
}

void loadConfig(int* array, int file_order) {
    switch (file_order) {
        case 1:
            fileReader(array, "configs/glaider.txt");
            break;
        case 2:
            fileReader(array, "configs/krest.txt");
            break;
        case 3:
            fileReader(array, "configs/bigPen.txt");
            break;
        case 4:
            fileReader(array, "configs/galactic.txt");
            break;
        case 5:
            fileReader(array, "configs/line.txt");
            break;
        default:
            printf("Error: can't load config.\n");
            break;
    }
}

void confHandler(int** lifes) {
    const int size = 1000;
    printf("Доступные конфиги: 1 - glaider, 2 - krest, 3 - bigPen, 4 - galactic, 5 - line\n");
    printf("Выберите конфиг: ");
    int response;
    short int flag = scanf("%d", &response);
    if (flag == 1) {
        int* array = (int*)malloc(size * sizeof(int));  // память под массив
        for (int i = 0; i < size; ++i) array[i] = -1;
        if (response >= 1 && response <= 5) {
            loadConfig(array, response);
            int i = 0;
            while (array[i] != -1) {
                lifes[array[i]][array[i + 1]] = 1;
                i += 2;
            }
        } else {
            puts("Неправильный номер конфига. Введите число от 1 до 5.");
        }
        free(array);
    } else {
        puts("Неправильный номер конфига. Введите число от 1 до 5.");
    }
}

void draw(int** life_field) {
    char current_char;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            current_char = (life_field[i][j] == 1) ? CELL_FULL : CELL_EMPTY;
            printf("%c", current_char);
            // if (current_char == CELL_FULL) printf("🦠");
            // else printf("💀");
        }
        if (i != ROWS - 1) printf("\n");
    }
}

void updateWeights(int** weights, int** lifes) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (lifes[i][j] == 1) {                            // cell is full -> update weights neigbors
                weights[(i + 1) % ROWS][j] += 1;               // вниз
                weights[i][(j + 1) % COLS] += 1;               // вправо
                weights[(i + 1) % ROWS][(j + 1) % COLS] += 1;  // вниз и вправо
                if (i == 0) {
                    weights[ROWS - 1][(j + 1) % COLS] += 1;  // вверх и вправо на границе
                    weights[ROWS - 1][j] += 1;               // вверх на границе
                } else {
                    weights[i - 1][(j + 1) % COLS] += 1;  // вверх и вправо в поле
                    weights[i - 1][j] += 1;               // вверх в пол
                }
                if (i == 0 && j == 0)
                    weights[ROWS - 1][COLS - 1] += 1;  // влево вверх если в самом углу
                else if (i == 0)
                    weights[ROWS - 1][j - 1] += 1;  // влево вверх если на границе сверху
                else if (j == 0)
                    weights[i - 1][COLS - 1] += 1;  // влево вверх если на границе слева
                else
                    weights[i - 1][j - 1] += 1;  // влево вверх если на поле
                if (j == 0) {
                    weights[i][COLS - 1] += 1;               // влево если на границе
                    weights[(i + 1) % ROWS][COLS - 1] += 1;  // влево вниз если на границе
                } else {
                    weights[i][j - 1] += 1;               // влево если в поле
                    weights[(i + 1) % ROWS][j - 1] += 1;  // влево вниз если на границе
                }
            }
        }
    }
}

void updateLifes(int** weights, int** lifes) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
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

int isThereLifeHere(int** lifes) {
    short int flag = 0;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (lifes[i][j]) {
                flag = 1;
                break;
            }
        }
    }
    return flag;
}

int allocateMatrix(int*** matrix) {
    short int flag = 1;
    *matrix = (int**)malloc(ROWS * sizeof(int*));
    if (*matrix == NULL) {
        flag = 0;  // any Errors
    }

    for (int i = 0; i < ROWS; i++) {
        (*matrix)[i] = (int*)malloc(COLS * sizeof(int));
        if ((*matrix)[i] == NULL) {
            flag = 0;  // any Errors
            break;
        }
    }

    return flag;
}

void freeMatrix(int*** matrix) {
    for (int i = 0; i < ROWS; i++) {
        if ((*matrix)[i] != NULL) {
            free((*matrix)[i]);
            (*matrix)[i] = NULL;
        }
    }

    if (*matrix != NULL) {
        free(*matrix);
        *matrix = NULL;
    }
}

void setZeros(int** matrix) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            matrix[i][j] = 0;
        }
    }
}

// ##########################################################MENU###########################################################################

void printBorderY() { printf("-"); }
void printBorderX() { printf("|"); }
void printEmpty() { printf(" "); }
void printGameSymbol() { printf("•"); }
void printDecor() { printf("*"); }
void clearScreen() { printf("\033[H\33[J"); }
short int lettersRander(const int row, const int col);
short int decorSymbol(const int row, const int col);

void menuRander() {
    for (int i = 0; i < ROWS; i++) {
        if (i == 0 || i == ROWS - 1) {
            for (int j = 0; j < COLS; j++) {
                printBorderY();
            }
            continue;
        }
        for (int j = 0; j < COLS; j++) {
            if (j == 0 || j == COLS - 1) {
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

void optionHandler() {
    int option;
    menuRander();
    scanf("%d", &option);
    switch (option) {
        case 1:
            clearScreen();
            run();
            break;
        case 2:
            printf("Good luck, have a great time :)\n");
            exit(0);
        default:
            printf("Choose the correct option.\n");
            optionHandler();
    }
}

int readInput() {
    int flag = -1;
    char ch;
    int stdin_fd = fileno(stdin);

    if (read(stdin_fd, &ch, sizeof(char)) == 1) {
        flag = ch;
    }

    return flag;  // Если нажатие не было обнаружено
}
void termOn() {
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
void termOff() {
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}