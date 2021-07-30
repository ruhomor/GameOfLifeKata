#include <stdlib.h>

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
    while (rows--) {
        while (cols--) {
            cells[rows][cols] = 0;
        }
    }
    return cells;
}

int         **new_universe(int rows, int cols) {
    int     **cells;
    int     **tmp;
    int     i;

    if (!(cells = (typeof(cells))malloc(sizeof(*cells) * (rows + 1))))
        return NULL;
    cells[rows] = NULL;
    tmp = cells;
    i = rows;
    while (i--) {
        if (!(*tmp = (typeof(*tmp))malloc(sizeof(**tmp) * (cols + 1))))
            return NULL;
        (*tmp)[cols] = -1;
        tmp++;
    }
    return init_universe(cells, rows, cols);
}

int         border_check(int coord, int *limit) {
    if (coord < 0) {
        *limit = 0;
        return 0;
    }
    if (coord >= *limit) {
        *limit = *limit - 1;
        return 0;
    }
    (*limit)--;
    return 1;
}

int         iter_cell(int **cells, int y, int x, int rows, int cols) {
    int     j, i;
    int     lim_y = rows, lim_x = cols;
    int     neighbours = 0;
    int     own_state;

    if ((own_state = border_check(y, &lim_y) * border_check(x, &lim_x))) { //border and self state check
        own_state = cells[y][x];
    }
    for (j = y - 1; j <= lim_y; j++) {
        for (i = x - 1; i <= lim_x; i++) {
            if ((cells[j][i]) && !((i == x) && (j == y))) { //self_check
                neighbours++;
            }
        }
    }
    return rules(neighbours, own_state);
}

int         **step(int **cells, int *rowptr, int *colptr) {
    int     j, i;
    int     *border;
    int     *tmp;

    if (!(border = (typeof(border))malloc(sizeof(*border) * (*rowptr + *colptr) * 2)))
        return NULL;
    tmp = border;
    for (j = -1; j <= *rowptr; j++) {
        *tmp = iter_cell(cells, j, -1, *rowptr, *colptr);
        tmp++;
        *tmp = iter_cell(cells, j, *colptr, *rowptr, *colptr);
        tmp++;
    }
    for (i = 0; i < *colptr; i++) {
        *tmp = iter_cell(cells, -1, i, *rowptr, *colptr);
        tmp++;
        *tmp = iter_cell(cells, *rowptr, i, *rowptr, *colptr);
        tmp++;
    }

    for (j = 0; j < *rowptr; j++) {
        for (i = 0; i < *colptr; i++) {

        }
    }
}

int         **get_generation(int **cells, int generations, int *rowptr, int *colptr) {

}

int         main() {
    int     **universe = new_universe(5, 5);

    universe[0][0] = 1; //#..
    universe[1][1] = 1; //.##
    universe[1][2] = 1; //##.
    universe[2][0] = 1;
    universe[2][1] = 1;

    return 0;
}
