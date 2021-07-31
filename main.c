#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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

int         iter_cell(int **cells, int y, int x, int rows, int cols) {
    int     j, i;
    int     lim_y = min(rows - 1, y + 1);
    int     lim_x = min(cols - 1, x + 1);
    int     neighbours = 0;
    int     own_state;

    if ((x < 0 || x > lim_x) || (y < 0 || y > lim_y))
        own_state = 0;
    else
        own_state = cells[y][x];

    for (j = max(0, y - 1); j <= lim_y; j++)
        for (i = max(0, x - 1); i <= lim_x; i++)
            if ((cells[j][i]) && (!((x == i) && (y == j))))
                neighbours++;

    return rules(neighbours, own_state);
}

void        populate_border(int **cells, int rows, int cols, //expand up-down
                            int *border, unsigned int expand) { //left-right
    int     i, j;
    int     *tmp;

    if ((expand & 0b1000) && (expand & 0b0010)) //up-left
        cells[0][0] = *border;
    if ((expand & 0b1000) && (expand & 0b0001)) //up-right
        cells[0][cols] = *(border + 1);
    if ((expand & 0b0100) && (expand & 0b0010)) //down-left
        cells[rows][0] = *(border + 2 * rows + 2);
    if ((expand & 0b0100) && (expand & 0b0001)) //down-right
        cells[rows][cols] = *(border + 2 * rows + 3);

    if (expand & 0b0010)
    {
        tmp = border + 2;
        for (j = (expand >> 3 & 0b0001); j < rows + (expand >> 2 & 0b0001); j++)
        {
            cells[j][0] = *tmp;
            tmp++;
            tmp++;
        }
    }
    if (expand & 0b0001)
    {
        tmp = border + 3;
        for (j = (expand >> 3 & 0b0001); j < rows + (expand >> 2 & 0b0001); j++)
        {
            cells[j][cols] = *tmp;
            tmp++;
            tmp++;
        }
    }
    if (expand & 0b1000)
    {
        tmp = border + 2 * rows + 4;
        for (i = (expand >> 1 & 0b0001); i < cols + (expand & 0b0001); i++)
        {
            cells[0][i] = *tmp;
            tmp++;
            tmp++;
        }
    }
    if (expand & 0b0100)
    {
        tmp = border + 2 * rows + 5;
        for (i = (expand >> 1 & 0b0001); i < cols + (expand & 0b0001); i++)
        {
            cells[rows][i] = *tmp;
            tmp++;
            tmp++;
        }
    }
}

int                 **step(int **cells, int *rowptr, int *colptr) {
    int             j, i;
    int             *border;
    int             *tmp;
    int             **new_cells;
    unsigned int    expand = 0b0000; //Up Down Left Right
    int             old_row, old_col;
    int             **tmp2;

    if (!(border = (typeof(border))malloc(sizeof(*border) * (*rowptr + *colptr) * 2 + 4)))
        return NULL;
    tmp = border;
    for (j = -1; j <= *rowptr; j++) {
        if ((*tmp = iter_cell(cells, j, -1, *rowptr, *colptr)))
            expand |= 0b0010;
        tmp++;
        if ((*tmp = iter_cell(cells, j, *colptr, *rowptr, *colptr)))
            expand |= 0b0001;
        tmp++;
    }
    for (i = 0; i < *colptr; i++) {
        if ((*tmp = iter_cell(cells, -1, i, *rowptr, *colptr)))
            expand |= 0b1000;
        tmp++;
        if ((*tmp = iter_cell(cells, *rowptr, i, *rowptr, *colptr)))
            expand |= 0b0100;
        tmp++;
    }

    old_row = *rowptr;
    old_col = *colptr;

    *colptr += (expand & 0b0001); //right
    *colptr += (expand >> 1 & 0b0001); //left
    *rowptr += (expand >> 2 & 0b0001); //down
    *rowptr += (expand >> 3 & 0b0001); //up

    new_cells = new_universe(*rowptr, *colptr); //this function could change row/col
    //populate_border(new_cells, old_row, old_col, border, expand);
    free(border);

    //TODO old universe

    for (j = (expand >> 3 & 0b0001); j < *rowptr; j++) {
        int     old_j = j - (expand >> 3 & 0b0001);
        for (i = (expand >> 1 & 0b0001); i < *colptr; i++) {
            int     old_i = i - (expand >> 1 & 0b0001);
            new_cells[j][i] = iter_cell(cells, old_j, old_i, old_row, old_col);
        }
    }

    tmp2 = cells; //memory freeing
    i = old_row;
    while (i--)
    {
        free(*cells);
        cells++;
    }
    free(tmp2);

    return new_cells;
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
