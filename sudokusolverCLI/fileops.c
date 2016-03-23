#include "main.h"

// if return 0 then fail, if return 1 then ok
int grid_from_file( char * filename, char grid[9][9] )
{
    FILE *fp;
    int i, x;

    fp = fopen(filename,"r");
    if( fp == NULL )
    {
        printf("File opening error!\n");
        return 0;
    }

    i = 0;
    while  ( ( x = fgetc( fp ) ) != EOF )
    {
        if( is_valid_symbol(x) )
        {
            if( i == 81 )
            {
                printf("File is too big\n");
                fclose(fp);
                return 0;
            }
            else
            {
                grid[i/9][i%9] = x;
                i++;
            }
        }
        else
        {
            if( x == 10 || x == 13 ) continue;
            printf("Incorrect symbol!\n");
            fclose(fp);
            return 0;
        }        
    }

    if( i != 81)
    {
        printf("File is too short!\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

//  return 1 - ok, return 0 - fail
int grid_to_file( char * filename, char grid[9][9] )
{
    FILE *fp;
    int i, j;

    fp = fopen(filename,"w");
    if( fp == NULL )
    {
        printf("File opening error!\n");
        return 0;
    }

    for( i = 0; i < 9; i++ )
    {
        for( j = 0; j < 9; j++ )
        {
            fputc( grid[i][j], fp );
        }
        fputc( '\n', fp );
    }

    fclose(fp);
    return 1;
}
