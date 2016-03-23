#include "main.h"

// return 1 - ok, return 0 - fail
int is_valid_symbol(int x)
{
    if( (x >= 49 && x <= 57) || x == 45 ) return 1;
    return 0;
}

// return 1 - ok, return 0 - fail
int is_valid_option_in_grid( char grid[9][9], int row, int col )
{
    int rowcheck_iter, colcheck_iter, row_square, col_square;

    // emptyness is valid option
    if( grid[row][col] == '-' ) return 1;

    // check row
    for( rowcheck_iter = 0; rowcheck_iter < 9; rowcheck_iter++ )
    {
        if( col == rowcheck_iter ) continue;
        if( grid[row][col] == grid[row][rowcheck_iter] ) return 0;
    }
    // check column
    for( colcheck_iter = 0; colcheck_iter < 9; colcheck_iter++ )
    {
        if( row == colcheck_iter ) continue;
        if( grid[row][col] == grid[colcheck_iter][col] ) return 0;
    }
    // check square
    row_square = row / 3;
    col_square = col / 3;
    for( rowcheck_iter = 0; rowcheck_iter < 3; rowcheck_iter++ )
    {
        for( colcheck_iter = 0; colcheck_iter < 3; colcheck_iter++ )
        {
            if( row == row_square*3 + rowcheck_iter && col == col_square*3 + colcheck_iter ) continue;
            if( grid[row][col] == grid[row_square*3 + rowcheck_iter][col_square*3 + colcheck_iter] ) return 0;
        }
    }
    return 1;
}

// if return is 0 - fail, if 1 - ok
int is_valid_grid(char grid[9][9])
{
    int i, j;
    for(i = 0; i < 9; i++)
    {
        for( j = 0; j < 9; j++ )
        {
            if( !is_valid_option_in_grid( grid, i, j ) ) return 0;
        }
    }
    return 1;
}