#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define LEFT 0b0001
#define RIGHT 0b0010
#define TOP 0b0100
#define BOTTOM 0b1000

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
                    neighbours++;
            }
            else
                own_state = cells[y][x];
        }
    }
    //printf("x: %d y: %d n: %d\n", x, y, neighbours);
    return rules(neighbours, own_state);
}

typedef struct      s_borders {
    int             *top_border;
    int             *bot_border;
    int             *left_border;
    int             *right_border;
}                   t_borders;

t_borders       eval_borders(int **cells, int rows, int cols) {
    t_borders   borders;
    int         j, i;

    borders.top_border = alloc_border(cols);
    borders.bot_border = alloc_border(cols);
    borders.left_border = alloc_border(rows);
    borders.right_border = alloc_border(rows);

    for (i = -1; i <= cols; i++) { //eval horizontal borders
        borders.top_border[i + 1] = iter_cell(cells, i, -1, cols, rows);
        borders.bot_border[i + 1] = iter_cell(cells, i, rows, cols, rows);
    }
    for (j = -1; j <= rows; j++) { //eval vertical borders
        borders.left_border[j + 1] = iter_cell(cells, -1, j, cols, rows);
        borders.right_border[j + 1] = iter_cell(cells, cols, j, cols, rows);
    }
    return borders;
}

void            destroy_universe(int **cells, int rows) {
    int         j;

    for (j = 0; j < rows; j++)
        free(cells[j]);
    free(cells);
}

int             **eval_self(int **cells, int rows, int cols) {
    int         **new_cells;
    int         j, i;

    new_cells = new_universe(rows, cols);
    for (j = 0; j < rows; j++) { //eval self
        for (i = 0; i < cols; i++) {
            new_cells[j][i] = iter_cell(cells, i, j, cols, rows); //optimize?
        }
    }
    destroy_universe(cells, rows);
    return new_cells;
}

unsigned int        check_border(int *border, int limit, unsigned int side) {
    int             i;

    for (i = 0; i < limit; i++) {
        if (border[i] == 1)
            return side;
    }
    return 0;
}

void                copy_universe_exp(int **src, int **dst, unsigned int expand, int old_rows, int old_cols) {
    int             j, i;
    int             shift_x, shift_y;

    shift_x = (expand & LEFT) ? 1 : 0;
    shift_y = (expand & TOP) ? 1 : 0;

    for (j = 0; j < old_rows; j++) {
        for (i = 0; i < old_cols; i++) {
            dst[j + shift_y][i + shift_x] = src[j][i];
        }
    }
}

void                populate_borders(int **cells, t_borders borders, unsigned int expand,
                                     int rows, int cols) {
    int             i;
    int             shift_y, shift_x;

    shift_y = (expand & TOP) ? 0 : 1;
    shift_x = (expand & LEFT) ? 0 : 1;

    if (expand & LEFT) { //if expanded left
        for (i = 0; i < cols; i++)
            cells[i][0] = borders.left_border[i + shift_y];
    }
    if (expand & TOP) { //if expanded top
        for (i = 0; i < rows; i++)
            cells[0][i] = borders.top_border[i + shift_x];
    }
    if (expand & RIGHT) {
        for (i = 0; i < cols; i++)
            cells[i][cols - 1] = borders.right_border[i + shift_y];
    }
    if (expand & BOTTOM) {
        for (i = 0; i < rows; i++)
            cells[rows - 1][i] = borders.bot_border[i + shift_x];
    }
}

int                 **expand(int **cells, t_borders borders, int *rowptr, int *colptr) {
    unsigned int    expand = 0;
    int             old_cols = *colptr, old_rows = *rowptr;
    int             **expanded_universe;

    expand |= check_border(borders.left_border, *colptr + 2, LEFT);
    expand |= check_border(borders.right_border, *colptr + 2, RIGHT);
    expand |= check_border(borders.top_border, *rowptr + 2, TOP);
    expand |= check_border(borders.bot_border, *rowptr + 2, BOTTOM);

    if (expand & LEFT)
        (*colptr)++;
    if (expand & RIGHT)
        (*colptr)++;
    if (expand & TOP)
        (*rowptr)++;
    if (expand & BOTTOM)
        (*rowptr)++;

    expanded_universe = new_universe(*rowptr, *colptr);
    copy_universe_exp(cells, expanded_universe, expand, old_rows, old_cols);
    destroy_universe(cells, old_rows);
    populate_borders(expanded_universe, borders, expand, *rowptr, *colptr);
    return expanded_universe;
}

typedef struct  s_blank_lines {
    int         top;
    int         bottom;
    int         left;
    int         right;
}               t_blank_lines;

int                 check_vertical(int **cells, int coord, int rows) {
    int             i;

    for (i = 0; i < rows; i++) {
        if (cells[i][coord] == 1)
            return 1;
    }
    return 0;
}

int                 check_horizontal(int **cells, int coord, int cols) {
    int             i;

    for (i = 0; i < cols; i++) {
        if (cells[coord][i] == 1)
            return 1;
    }
    return 0;
}

void                copy_universe_con(int **src, int **dst, t_blank_lines lines, int rows, int cols) {
    int             j, i;

    for (j = 0; j < rows; j++) {
        for (i = 0; i < cols; i++) {
            dst[j][i] = src[j + lines.top][i + lines.left];
        }
    }
}

int                 **contract(int **cells, int *rowptr, int *colptr) {
    t_blank_lines   lines;
    int             i;
    int             **contracted_universe;

    lines.left = 0;
    lines.right = 0;
    lines.top = 0;
    lines.bottom = 0;

    for (i = 0; i < *rowptr; i++) {
        if (check_horizontal(cells, i, *colptr))
            break;
        lines.top++;
    }
    for (i = *rowptr - 1; i >= 0; i--) {
        if (check_horizontal(cells, i, *colptr))
            break;
        lines.bottom++;
    }
    for (i = 0; i < *colptr; i++) {
        if (check_vertical(cells, i, *rowptr))
            break;
        lines.left++;
    }
    for (i = *colptr - 1; i >= 0; i--) {
        if (check_vertical(cells, i, *rowptr))
            break;
        lines.right++;
    }
    *rowptr -= lines.top + lines.bottom;
    *colptr -= lines.left + lines.right;
    contracted_universe = new_universe(*rowptr,*colptr);
    copy_universe_con(cells, contracted_universe, lines, *rowptr, *colptr);
    destroy_universe(cells, *rowptr + lines.top + lines.bottom);
    return contracted_universe;
}

int             **step(int **cells, int *rowptr, int *colptr) {
    //plan
    //eval and store border
    //eval self
    //expand
    //populate border
    //contract
    t_borders   borders;

    borders = eval_borders(cells, *rowptr, *colptr);
    cells = eval_self(cells, *rowptr, *colptr);
    cells = expand(cells, borders, rowptr, colptr);
    cells = contract(cells, rowptr, colptr);
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
    print_universe(universe, rows, cols);
    write(1, "\n", 1);
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
