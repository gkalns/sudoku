#ifndef SUDOKUENGINE_H
#define SUDOKUENGINE_H

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector

#include <QProgressDialog>


class SudokuEngine
{
public:
    SudokuEngine();
    char grid[9][9];
    char temp_grid[9][9];

    int is_valid_grid();
    int dumbsolve();
    int solution_count();
    int generate();

private:
    int increasepos(int);
    int backoff(int);
    int is_valid_option_in_grid( char thisgrid[9][9], int, int );
    int is_valid_symbol(int);
};

#endif // SUDOKUENGINE_H
