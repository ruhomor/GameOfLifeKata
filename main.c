#include <stdlib.h>
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
    while (rows--) {
        while (cols--) {
            cells[rows][cols] = 0;
        }
    }
    return cells;
}

void        print_universe(int **cells, int rows, int cols) {
    char    *str = (typeof(str))malloc(sizeof(*str) * cols); //TODO no memalloc check
    int     i, j;

    for (j = 0; j < rows; j++)
    {
        for (i = 0; i < cols; i++)
            *str = cells[j][i] + 48; //on purpose
        write(1, str, cols);
    }
    free(str); //might crash because again TODO no memalloc check
}

int         **new_universe(int rows, int cols) {
    int     **cells;
    int     **tmp;
    int     i;

    if (!(cells = (typeof(cells))malloc(sizeof(*cells) * (rows))))
        return NULL;
    //cells[rows] = NULL; // hellLL
    tmp = cells;
    i = rows;
    while (i--) {
        if (!(*tmp = (typeof(*tmp))malloc(sizeof(**tmp) * (cols))))
            return NULL;
        //(*tmp)[cols] = -1; //unsafe as hell
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
        *limit -= 1;
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

void        populate_border(int **cells, int rows, int cols, //expand up-down
                            int *border, unsigned int expand) { //left-right
    int     i, j;
    int     *tmp;
    int     lim_x = cols - 1, lim_y = rows - 1;

    if ((expand & 0b1000) && (expand & 0b0010)) //up-left
        cells[0][0] = *border;
    if ((expand & 0b1000) && (expand & 0b0001)) //up-right
        cells[0][lim_x] = *(border + 1);
    if ((expand & 0b0100) && (expand & 0b0010)) //down-left
        cells[lim_y][0] = *(border + 2 * rows - 2);
    if ((expand & 0b0100) && (expand & 0b0001)) //down-right
        cells[lim_y][lim_x] = *(border + 2 * rows - 1);

    tmp = border + 1;
    if (expand & 0b0010)
    {
        for (j = 1; j < lim_y; j++)
        {
            cells[j][0] = *tmp;
            tmp++;
            tmp++;
        }
    }
    tmp = border + 2;
    if (expand & 0b0001)
    {
        for (j = 1; j < lim_y; j++)
        {
            cells[j][lim_x] = *tmp;
            tmp++;
            tmp++;
        }
    }
    tmp = border + 2 * rows;
    if (expand & 0b1000)
    {
        for (i = 1; i < lim_x; i++)
        {
            cells[0][i] = *tmp;
            tmp++;
            tmp++;
        }
    }
    tmp = border + 2 * rows + 1;
    if (expand & 0b1000)
    {
        for (i = 1; i < lim_x; i++)
        {
            cells[lim_y][i] = *tmp;
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

    if (!(border = (typeof(border))malloc(sizeof(*border) * (*rowptr + *colptr) * 2)))
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

    *colptr += (expand & 0b0001); //right
    *colptr += (expand >> 1 & 0b0001); //left
    *rowptr += (expand >> 2 & 0b0001); //down
    *rowptr += (expand >> 3 & 0b0001); //up

    old_row = *rowptr;
    old_col = *colptr;

    new_cells = new_universe(*rowptr, *colptr); //this function could change row/col
    populate_border(new_cells, *rowptr, *colptr, border, expand);

    for (j = 0; j < old_row; j++) {
        for (i = 0; i < old_col; i++) {
            new_cells[j + (expand >> 2 & 0b0001)][i + (expand >> 1 & 0b0001)]
            = iter_cell(cells, j, i, old_row, old_col);
        }
    }
    return new_cells;
}

int         **get_generation(int **cells, int generations, int *rowptr, int *colptr) {
    while (generations--)
        cells = step(cells, rowptr, colptr);
    return cells;
}

int         main() {
    int     **universe = new_universe(5, 5);
    int     rows = 5, cols = 5;

    universe[0][0] = 1; //#..
    universe[1][1] = 1; //.##
    universe[1][2] = 1; //##.
    universe[2][0] = 1;
    universe[2][1] = 1;

    print_universe(get_generation(universe, 1, &rows, &cols), rows, cols);
    print_universe(get_generation(universe, 2, &rows, &cols), rows, cols);
    return 0;
}
