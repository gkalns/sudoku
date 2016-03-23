#ifndef SUDOKUSOLVER_dumbsolve
#define SUDOKUSOLVER_dumbsolve

int increasepos(int, char solution_grid[9][9]);
int backoff(int, char solution_grid[9][9], char grid[9][9]);
int dumbsolve( char grid[9][9], char solution_grid[9][9] );

#endif