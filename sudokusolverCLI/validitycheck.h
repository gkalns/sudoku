#ifndef SUDOKUSOLVER_validitycheck
#define SUDOKUSOLVER_validitycheck

int is_valid_symbol(int);
int is_valid_option_in_grid( char grid[9][9], int, int );
int is_valid_grid(char grid[9][9]);

#endif