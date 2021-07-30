#include <stdlib.h>

int         rules(int neighbours, int own_state) {
    switch(n) {
        case constant-expression  :
            statement(s);
            break;
            case constant-expression  :
                statement(s);
                break; /* optional */

                /* you can have any number of case statements */
                default : /* Optional */
                statement(s);
    }
}

int         init_universe(int **cells, int rows, int cols) {
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

/*
int         min(int a, int b) {
    return a < b ? a : b;
}

int         max(int a, int b) {
    return a > b ? a : b;
}
*/

int         border_check(int coord, int limit) {
    return
}

int         iter_cell(int **cells, int y, int x, int rows, int cols) {
    int     j, i;
    /*
    int     lim_x = max(0, min(x + 1, --cols)); //border_check
    int     lim_y = max(0, min(y + 1, --rows)); //border_check
    */
    int     neighbours = 0;
    int     own_state;

    for(j = y - 1; j <= lim_y; j++) {
        for(i = x - 1; i <= lim_x; i++) {
            if ((cells[j][i]) && !((i == x) && (j == y))) { //self_check
                neighbours++;
            }
        }
    }
    return rules(neighbours, cells[y][x]);
}

int         **step(int **cells, int *rowptr, int *colptr) {

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
