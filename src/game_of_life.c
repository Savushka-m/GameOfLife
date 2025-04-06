#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 25
#define COLS 80

void logic_function(int **main_matrix);
void draw_world(int **main_matrix);
void speed(int *pause, int *quit);

int main() {
    int **main_matrix = malloc(ROWS * COLS * sizeof(int) + ROWS * sizeof(int *));
    int *ptr = (int *)(main_matrix + ROWS);
    for (int i = 0; i < COLS; i++) main_matrix[i] = ptr + COLS * i;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
            if (scanf("%d", &main_matrix[i][j]) != 1) {
                printw("n/a");
                return 1;
            }
    }

    (void)!freopen("/dev/tty", "r", stdin);
    int time_sec = 500;  // начальное время задержки
    int quit = 0;
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 1);  // Делаем getch неблокирующим

    // Игровой цикл
    while (!quit) {
        speed(&time_sec, &quit);
        usleep((time_sec + 15) * 1000);
        clear();
        printw("Speed = %d\n", (5000 - time_sec) / 100);
        draw_world(main_matrix);
        logic_function(main_matrix);
    }

    endwin();           // Завершаем режим ncurses
    free(main_matrix);  // Освобождаем выделенную память
    return 0;
}

void speed(int *pause, int *quit) {
    char speed = getch();  // Получаем ввод пользователя
    switch (speed) {
        case '-':
            if (*pause <= 5000) {
                *pause += 100;
            }
            break;
        case '+':
            if (*pause >= 100) {
                *pause -= 100;
            }
            break;
        case '1':
            *pause = 300;
            break;
        case '2':
            *pause = 600;
            break;
        case '3':
            *pause = 900;
            break;
        case '4':
            *pause = 1200;
            break;
        case 'q':
            *quit = 1;  // Устанавливаем флаг выхода в истину
            break;
    }
}

void draw_world(int **main_matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (main_matrix[i][j] == 0)
                printw(" ");
            else
                printw("o");
            if (j == COLS - 1) printw("\n");
        }
    }
}

void logic_function(int **main_matrix) {
    /*создаем временную матрицу*/
    int x, y, sum_alive;
    int **temp_matrix = malloc(ROWS * COLS * sizeof(int) + ROWS * sizeof(int *));
    int *ptr = (int *)(temp_matrix + ROWS);
    for (int i = 0; i < COLS; i++) temp_matrix[i] = ptr + COLS * i;
    /*заполняем временную матрицу по следующему принципу:
      в ячейку temp[i][j] записывает количество живых соседей для клетки main[i][j]*/
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            sum_alive = 0;
            for (int k = i - 1; k <= i + 1; k++)
                for (int l = j - 1; l <= j + 1; l++) {
                    y = (k + ROWS) % ROWS;
                    x = (l + COLS) % COLS;
                    sum_alive += main_matrix[y][x];
                }
            sum_alive -= main_matrix[i][j];
            temp_matrix[i][j] = sum_alive;
        }
    /*Сравниваем значения исходной матрицы, учитывая значения из временной, прописывая правила:
      1) если у мертвой клетки РОВНО 3 живых соседа, то делаем её живой
      2) если у живой клетки 2 или 3 живых соседа, то оставляем её живой
      3) если у живой клетки больше 2 или меньше 3 соседей, то делаем её мертвой
      4) остальные клетки оставляем мертвыми*/
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            if (main_matrix[i][j] == 0 && temp_matrix[i][j] == 3)
                main_matrix[i][j] = 1;
            else if (main_matrix[i][j] == 1 && (temp_matrix[i][j] == 2 || temp_matrix[i][j] == 3))
                main_matrix[i][j] = 1;
            else if (main_matrix[i][j] == 1 && (temp_matrix[i][j] < 2 || temp_matrix[i][j] > 3))
                main_matrix[i][j] = 0;
            else
                main_matrix[i][j] = 0;
        }
    /*Не забываем освободить память выделенную нашей временной матрице*/
    free(temp_matrix);
}
