#include "sudokuengine.h"

SudokuEngine::SudokuEngine()
{
    memset(grid, 0, sizeof(grid));
    srand (time(NULL)); // initialize random seed for random number gen
}

// if return is 0 - fail (no solutions), if 1 - ok
int SudokuEngine::dumbsolve()
{
    int i = 0;

    // temp_grid is not touched. It is used to check where are empty places
    memcpy(temp_grid, grid, sizeof (char) * 81);

    while( i != 81 )
    {
        if( temp_grid[ i/9 ][ i%9 ] != '\0' )
        {
            i++;
            continue;
        }
        if( increasepos(i) )
        {
            i++;
        }
        else
        {
            i = backoff(i);
            if( i == -1 )
            {
                return 0;
            }
            i++;
        }
    }
    return 1;
}

// if return is 0 - fail, if 1 - ok
int SudokuEngine::is_valid_grid()
{
    int i, j;
    for(i = 0; i < 9; i++)
    {
        for( j = 0; j < 9; j++ )
        {
            if( !this->is_valid_symbol( grid[i][j] ) ) return 0;
            if( !this->is_valid_option_in_grid( grid, i, j ) ) return 0;
        }
    }
    return 1;
}

// return 1 - ok, return 0 - fail
// valid symbols are 1 - 9 and \0 for blank space
int SudokuEngine::is_valid_symbol(int x)
{
    if( (x >= 49 && x <= 57) || x == 0 ) return 1;
    return 0;
}

// return 1 - ok, return 0 - fail
int SudokuEngine::is_valid_option_in_grid( char thisgrid[9][9], int row, int col )
{
    int rowcheck_iter, colcheck_iter, row_square, col_square;

    // emptyness is valid option
    if( thisgrid[row][col] == '\0' ) return 1;

    // check row
    for( rowcheck_iter = 0; rowcheck_iter < 9; rowcheck_iter++ )
    {
        if( col == rowcheck_iter ) continue;
        if( thisgrid[row][col] == thisgrid[row][rowcheck_iter] ) return 0;
    }
    // check column
    for( colcheck_iter = 0; colcheck_iter < 9; colcheck_iter++ )
    {
        if( row == colcheck_iter ) continue;
        if( thisgrid[row][col] == thisgrid[colcheck_iter][col] ) return 0;
    }
    // check square
    row_square = row / 3;
    col_square = col / 3;
    for( rowcheck_iter = 0; rowcheck_iter < 3; rowcheck_iter++ )
    {
        for( colcheck_iter = 0; colcheck_iter < 3; colcheck_iter++ )
        {
            if( row == row_square*3 + rowcheck_iter && col == col_square*3 + colcheck_iter ) continue;
            if( thisgrid[row][col] == thisgrid[row_square*3 + rowcheck_iter][col_square*3 + colcheck_iter] ) return 0;
        }
    }
    return 1;
}


// on success return 1, fail 0
int SudokuEngine::increasepos(int i)
{
    if( grid[ i/9 ][ i%9 ] == '\0' )
    {
        grid[ i/9 ][ i%9 ] = '1';
    }
    else if( grid[ i/9 ][ i%9 ] == '9' )
    {
        grid[ i/9 ][ i%9 ] = '\0';
        return 0;
    }
    else
    {
        grid[ i/9 ][ i%9 ]++;
    }

    if( this->is_valid_option_in_grid( grid, i/9, i%9 ) )
    {
        return 1;
    }
    else
    {
        return this->increasepos( i );
    }
}

// return new i or -1 in case no backup possible
int SudokuEngine::backoff(int i)
{
    do {
        i--;
        if( i == -1 ) return -1;
    } while( temp_grid[ i/9 ][ i%9 ] != '\0' );

    if( this->increasepos(i) )
    {
        return i;
    }
    else
    {
        return this->backoff(i);
    }

}

// return 0 if no solution, return 1 if just 1 solution, return 2 if more than 1 solution
int SudokuEngine::solution_count()
{
    int i = 0;
    bool solution = false;

    memcpy(temp_grid, grid, sizeof (char) * 81);

    while( 1 )
    {
        if( temp_grid[ i/9 ][ i%9 ] != '\0' )
        {
            i++;
            if( i == 81 )
            {
                if( !solution )
                {
                    solution = true;
                    do {
                        i--;
                        if( i == -1 ) return solution;
                    } while( temp_grid[ i/9 ][ i%9 ] != '\0' );
                    continue;
                }
                else
                {
                    memcpy(grid, temp_grid, sizeof (char) * 81);
                    return 2;
                }
            }
            continue;
        }
        if( increasepos(i) )
        {
            i++;
            if( i == 81 ) // if i = 81 then solution found
            {
                if( !solution )
                {
                    solution = true;
                    i--;
                    continue;
                }
                else
                {
                    memcpy(grid, temp_grid, sizeof (char) * 81);
                    return 2;
                }
            }
        }
        else
        {
            i = backoff(i);
            if( i == -1 )
            {
                // no need to memcpy grid back, it already is
                return solution;
            }
            i++;
        }
    }
}

int SudokuEngine::generate()
{
    // clear the playground
    memset(grid, 0, sizeof(grid));
    int pos = 0, number;

    // Generate random full grid (quite fast ~30ms)
    while( pos < 81 ){
        number = rand() % 9 + 49;
        do{
            do{
                number++;
                if( number == 58 ) number = 49;
                grid[ pos/9 ][ pos%9 ] = number;
            } while( !is_valid_option_in_grid( grid, pos/9, pos%9 ) );
        } while( !solution_count() );
        pos++;
    }

    // shuffle pos vector - in this order fields will get cleared
    std::vector<int> shuffled_poslist;
    for (int i=0; i<81; ++i) shuffled_poslist.push_back(i);
    std::random_shuffle ( shuffled_poslist.begin(), shuffled_poslist.end() );

    // It can be that checking solvability takes several seconds
    QProgressDialog progress("Generating grid...", "Cancel", 0, 81);
    progress.setWindowModality(Qt::WindowModal);

    // clean, leave just key numbers - can be slow
    for( int i=0; i < 81; i++ ){
        pos = shuffled_poslist.at(i);
        int num = grid[ pos/9 ][ pos%9 ];

        grid[ pos/9 ][ pos%9 ] = 0;
        if( solution_count() == 2 ){
            grid[ pos/9 ][ pos%9 ] = num;
        }
        progress.setValue(i);
        if (progress.wasCanceled()) break;
    }


//// Previous algorithm (slower) ////

//    int res = 2;
//    while( res != 1 ){

//        if( res == 0 ){
//            // in last position change number
//            do{
//                number++;
//                if( number == 58 ) number = 49;
//                grid[ pos/9 ][ pos%9 ] = number;
//            } while( !is_valid_option_in_grid( grid, pos/9, pos%9 ) );

//        } else {
//            do{ pos = rand() % 81; } while( grid[ pos/9 ][ pos%9 ] );

//            number = rand() % 9 + 49;
//            grid[ pos/9 ][ pos%9 ] = number;
//            while( !is_valid_option_in_grid( grid, pos/9, pos%9 ) ){
//                number++;
//                if( number == 58 ) number = 49;
//                grid[ pos/9 ][ pos%9 ] = number;
//            }
//        }
//        res = solution_count();
//    }

//    qDebug("time: %lld", timer.elapsed());

//    // clean, leve just key numbers
//    pos = 0;
//    while( pos < 81 ){
//        int num = grid[ pos/9 ][ pos%9 ];
//        if( !num ){ pos++; continue; }

//        grid[ pos/9 ][ pos%9 ] = 0;
//        if( solution_count() == 2 ){
//            grid[ pos/9 ][ pos%9 ] = num;
//        }
//        pos++;
//    }

    return 0;
}










