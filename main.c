#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int         rules(int neighbours, int own_state) {
    if (own_state) //alive rules
    {
        if (neighbours == 2 || neighbours == 3)
            return 1;
    }
    else //dead rules
    {
        if (neighbours == 3)
            return 1;
    }
    return 0;
}

int         **init_universe(int **cells, int rows, int cols) {
    int     i;

    while (rows--) {
        i = cols;
        while (i--) {
            cells[rows][i] = 0;
        }
    }
    return cells;
}

void        print_universe(int **cells, int rows, int cols) {
    char    *str = (typeof(str))malloc(sizeof(*str) * cols); //TODO no memalloc check
    char    *tmp = str;
    int     i, j;

    for (j = 0; j < rows; j++)
    {
        str = tmp;
        for (i = 0; i < cols; i++)
        {
            *str = cells[j][i] + 48; //on purpose
            str++;
        }
        write(1, tmp, cols);
        write(1, "\n", 1);
    }
    free(tmp); //might crash because again TODO no memalloc check
}

int         **new_universe(int rows, int cols) {
    int     **cells;
    int     **tmp;
    int     i;

    if (!(cells = (typeof(cells))malloc(sizeof(*cells) * (rows))))
        return NULL;
    tmp = cells;
    i = rows;
    while (i--) {
        if (!(*tmp = (typeof(*tmp))malloc(sizeof(**tmp) * (cols))))
            return NULL;
        tmp++;
    }
    return init_universe(cells, rows, cols);
}

int         min(int a, int b) {
    return a < b ? a : b;
}

int         max(int a, int b) {
    return a > b ? a : b;
}

int         *alloc_border(int size) {
    return (int*)malloc(sizeof(int) * (size + 2)); //+2 for possible expansion
}

int         iter_cell(int **cells, int x, int y, int cols, int rows) {
    int     own_state = 0;
    int     neighbours = 0;
    int     j, i;

    cols = min(cols - 1, x + 1);
    rows = min(rows - 1, y + 1);
    for (j = max(0, y - 1);  j <= rows; j++) {
        for (i = max(0, x - 1); i <= cols; i++) {
            if (!((i == x) && (j == y))) {
                if (cells[j][i] == 1)
                {
                    neighbours++;
                    if ((x == 1) && (y == 1))
                        printf("NEI: i: %d j: %d\n", i, j);
                }
            }
            else
                own_state = cells[y][x];
        }
    }
    //printf("x: %d y: %d n: %d\n", x, y, neighbours);
    return rules(neighbours, own_state);
}

int         **step(int **cells, int *rowptr, int *colptr) {
    //plan
    //eval and store border
    //eval self
    //expand
    //populate border
    //contract
    int     **new_cells = new_universe(*rowptr, *colptr);
    int     *top_border = alloc_border(*colptr);
    int     *bot_border = alloc_border(*colptr);
    int     *left_border = alloc_border(*rowptr);
    int     *right_border = alloc_border(*rowptr);
    int     i, j;

    for (i = -1; i <= *colptr; i++) { //eval horizontal borders
        top_border[i] = iter_cell(cells, i, -1, *colptr, *rowptr);
        bot_border[i] = iter_cell(cells, i, *rowptr, *colptr, *rowptr);
    }
    for (j = -1; j <= *rowptr; j++) { //eval vertical borders
        left_border[j] = iter_cell(cells, -1, j, *colptr, *rowptr);
        right_border[j] = iter_cell(cells, *colptr, j, *colptr, *rowptr);
    }
    for (j = 0; j < *rowptr; j++) { //eval self
        for (i = 0; i < *colptr; i++) {
            new_cells[j][i] = iter_cell(cells, i, j, *colptr, *rowptr); //optimize?
        }
    }
    return cells;
}

int         **get_generation(int **cells, int generations, int *rowptr, int *colptr) {
    while (generations--)
        cells = step(cells, rowptr, colptr);
    return cells;
}

int         main() {
    int     rows = 3, cols = 3;
    int     **universe = new_universe(rows, cols);
    int     **tmp;
    int     **tmp2;
    int     i;

    universe[0][1] = 1; //.#. DOWN TEST
    universe[1][2] = 1; //..#
    universe[2][0] = 1; //###
    universe[2][1] = 1;
    universe[2][2] = 1;

/*
    universe[0][2] = 1; //..# RIGHT TEST
    universe[1][0] = 1; //#.#
    universe[1][2] = 1; //.##
    universe[2][1] = 1;
    universe[2][2] = 1;
*/
/*
    universe[0][0] = 1; //#.. LEFT TEST
    universe[1][0] = 1; //#.#
    universe[1][2] = 1; //##.
    universe[2][0] = 1;
    universe[2][1] = 1;
*/
/*
    universe[0][0] = 1; //### UP TEST
    universe[0][1] = 1; //#..
    universe[0][2] = 1; //.#.
    universe[1][0] = 1;
    universe[2][1] = 1;
*/
    i = rows;
    tmp = universe;
    while (1) {
        print_universe(tmp = get_generation(tmp, 1, &rows, &cols), rows, cols);
        write(1, "\n", 1);
        getchar();
    }
    tmp2 = tmp;
    while (i--)
    {
        free(*tmp);
        tmp++;
    }
    free(tmp2);
    return 0;
}
