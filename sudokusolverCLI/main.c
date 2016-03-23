/*
    Author: gkalns

    Sudoku solving program.
    Arguments: filename of file containing an incompleted sudoku puzzle. In the file grid consists of 9 symbols in 9 rows. 
    Possible symbols are known digits and dashes representing empty spaces.
    2nd argument is optional and is a file to write the solution to.
    Program searches the given grid for the first combination without conflicting numbers. If a puzzle is valid then there is just one solution. 

*/


#include "main.h"


void print_grid(char grid[9][9]);

int main(int argc, char **argv)
{
    char grid[9][9];
    char solution_grid[9][9];
    struct timeval stop, start;

    if( !(argc == 2 || argc == 3) )
    {
        printf("Incorrect usage!\n");
        return -1;
    }

    // read grid from file
    if( !grid_from_file( argv[1], grid ) )
    {
        printf("Error in reading from file!\n");
        return -1;
    }

    // print the current grid
    printf("Grid:\n");
    print_grid(grid);

    // check if grid is valid
    if( !is_valid_grid(grid) )
    {
        printf("Invalid grid!\n");
        return -1;
    }

    gettimeofday(&start, NULL);

    // solve with searching
    if( !dumbsolve(grid, solution_grid) )
    {
        printf("No valid solution!\n");
        return -1;
    }

    gettimeofday(&stop, NULL);

    // print solution grid
    printf("Solution:\n");
    print_grid(solution_grid);
    printf("Solving time: %lu us\n", stop.tv_usec - start.tv_usec);

    if( argc == 3 )
    {
        if( !grid_to_file( argv[2], solution_grid ) ) printf("Failed writing to file\n");
    } 

    return 0;
}

void print_grid(char grid[9][9])
{
    int i,j;

    printf("-------------------------------------\n");
    for( i = 0; i < 9; i++ )
    {
        printf("|  ");
        for( j = 0; j < 9; j++ )
        {
            printf("%c", grid[i][j]);
            printf("  ");
            if( j == 2 || j == 5 || j == 8)
            {
                printf("|  ");
            }
        }
        printf("\n");
        if( i == 2 || i == 5 || i == 8 )
        {
            printf("-------------------------------------\n");
        }
    }
}
