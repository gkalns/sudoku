#include "main.h"

// on success return 1, fail 0
int increasepos(int i, char solution_grid[9][9])
{
    if( solution_grid[ i/9 ][ i%9 ] == '-' )
    {
        solution_grid[ i/9 ][ i%9 ] = '1';
    }
    else if( solution_grid[ i/9 ][ i%9 ] == '9' )
    {
        solution_grid[ i/9 ][ i%9 ] = '-';
        return 0;
    }
    else
    {
        solution_grid[ i/9 ][ i%9 ]++;
    }

    if( is_valid_option_in_grid( solution_grid, i/9, i%9 ) )
    {
        return 1;
    }
    else
    {
        return increasepos( i, solution_grid );
    }

}

// return new i or -1 in case no backup possible
int backoff(int i, char solution_grid[9][9], char grid[9][9])
{
    do {
        i--;
        if( i == -1 ) return -1;
    } while( grid[ i/9 ][ i%9 ] != '-' );

    if( increasepos(i, solution_grid ) )
    {
        return i;
    }
    else
    {
        return backoff(i, solution_grid, grid);
    }

}

// if return is 0 - fail, if 1 - ok
int dumbsolve( char grid[9][9], char solution_grid[9][9] )
{
    int i;

    memcpy(solution_grid, grid, sizeof (char) * 81);

    i = 0;
    while( i != 81 )
    {
        if( grid[ i/9 ][ i%9 ] != '-' )
        {
            i++;
            continue;
        }
        if( increasepos(i, solution_grid) )
        {
            i++;
        }
        else
        {
            i = backoff(i, solution_grid, grid);
            if( i == -1 )
            {
                printf("Unsolvable grid\n");
                return 0;
            }
            i++;
        }
    }
    return 1;
}