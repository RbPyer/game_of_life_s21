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
#define CELL_FULL '#'    // char -> #
#define CELL_EMPTY ' '  // char -> ·

// file
#define FILE_PATH "configs/"

// options for menu
#define OPTIONS "Enter option:\n1 - Start\n2 - Exit\n"

struct termios term;
//###########################################################FUNCS#############################################################################

// memory
int allocateMatrix(int*** matrix);
void freeMatrix(int*** matrix);

// draw
void draw(int** life_field);
void norm_draw(int** matrix);

// logic
void update_weights(int** weights, int** lifes);
void update_lifes(int** weights, int** lifes);
int is_there_life_here(int** lifes);
void setZeros(int** matrix);

// game
void run();
void term_on();
void term_off();
int read_input();

// file and configs
int fileReader(int* array, const char* file_name);
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
void menu_rander();
void option_handler();

// ##########################################################BODY###########################################################################
int main() {
    option_handler();

    return 0;
}

void run() {
    int** weights;
    int** lifes;
    int SLEEP_COUNT = 20000;
    // alloc fields
    if (allocateMatrix(&weights) && allocateMatrix(&lifes)) {
        setZeros(weights);
        setZeros(lifes);
        // load config
        confHandler(lifes);
        term_on();
        int stdin_fd = fileno(stdin);
        fcntl(stdin_fd, F_SETFL, O_NONBLOCK);  // Включение неблокирующего режима чтения
        // main loop
        int iter = 0;
        while (1) {  // пока есть клетки или счетчик итераций больше чем число
            // основное тело цикла
            // norm_draw(weights);

            int ch = read_input();
            read(stdin_fd, &ch, sizeof(char));
            if (ch == 'a' && SLEEP_COUNT < 10e5) {
                SLEEP_COUNT += 20000;
            }
            if (ch == 'z' && SLEEP_COUNT > 0) {
                SLEEP_COUNT -= 10000;
            }
            if (ch == 'q') {
                break;
            }
            draw(lifes);
            //printf("\n\n");

            setZeros(weights);
            update_weights(weights, lifes);
            update_lifes(weights, lifes);

            ++iter;
            
            usleep(SLEEP_COUNT);
            clearScreen();
        }
        term_off();
    } else
        printf("n/a");
    // free fields
    freeMatrix(&weights);
    freeMatrix(&lifes);
}



int fileReader(int* array, const char* file_name) {
    short int flag = 0;
    FILE* file;
    // Открыть файл для чтения
    file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл.\n");
        flag = 1;
    }
    // Определить размер файла
    fseek(file, 0, SEEK_END);
    rewind(file);
    // Выделить память для хранения чисел
    if (array == NULL) {
        printf("Ошибка выделения памяти.\n");
        flag = 1;
    }
    // Считать числа из файла
    int i = 0;
    while (fscanf(file, "%d", &array[i]) != EOF) {
        i++;
    }
    // Освободить память и закрыть файл
    fclose(file);
    return flag;
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
            fileReader(array, "configs/penis.txt");
            break;
        case 4:
            fileReader(array, "configs/cock_edit.txt");
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
    printf(
        "Доступные конфиги: 1 - glaider, 2 - krest, 3 - penis, 4 - cock_galactic, 5 - line, 6 - от руки\n");
    printf("Выберите конфиг: ");
    int response;
    scanf("%d", &response);

    int* array = (int*)malloc(size * sizeof(int));  // память под массив
    for (int i = 0; i < size; ++i) array[i] = -1;

    if (response >= 1 && response <= 5) {
        loadConfig(array, response);
    } else {
        // ввод от руки
    }
    int i = 0;
    while (array[i] != -1) {
        lifes[array[i]][array[i + 1]] = 1;
        i += 2;
    }

    free(array);
}

void draw(int** life_field) {
    char current_char = '!';
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            current_char = (life_field[i][j] == 1) ? CELL_FULL : CELL_EMPTY;
            
                printf("%c", current_char);
        }
        if (i != ROWS - 1) printf("\n");
    }
}

// void norm_draw(int** matrix) {
//     printf("\n\ndebug\n\n");
//     for (int i = 0; i < ROWS; ++i) {
//         for (int j = 0; j < COLS; ++j) {
//             if (j == COLS - 1)
//                 printf("%d ", matrix[i][j]);
//             else
//                 printf("%d", matrix[i][j]);
//         }
//         if (i != ROWS - 1) printf("\n");
//     }
//     printf("\n");
// }

void update_weights(int** weights, int** lifes) {
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

void update_lifes(int** weights, int** lifes) {
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

int is_there_life_here(int** lifes) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (lifes[i][j]) return 1;
        }
    }
    return 0;
}

int allocateMatrix(int*** matrix) {
    *matrix = (int**)malloc(ROWS * sizeof(int*));
    if (*matrix == NULL) {
        return 0;  // any Errors
    }

    for (int i = 0; i < ROWS; i++) {
        (*matrix)[i] = (int*)malloc(COLS * sizeof(int));
        if ((*matrix)[i] == NULL) {
            return 0;  // any Errors
        }
    }

    return 1;
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

void menu_rander() {
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

void option_handler() {
    int option;
    menu_rander();
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
            option_handler();
    }
}

int read_input() {
    int flag = -1;
    char ch;
    int stdin_fd = fileno(stdin);

    if (read(stdin_fd, &ch, sizeof(char)) == 1) {
        flag = ch;
    }

    return flag;  // Если нажатие не было обнаружено
}
void term_on() {
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
void term_off() {
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}