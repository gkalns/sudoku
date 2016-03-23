#ifndef IOGRID_H
#define IOGRID_H

#include <QString>
#include <QFile>
#include <QPixmap>

// for image ocr
#include <tesseract/baseapi.h>
//#include <leptonica/allheaders.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class iogrid
{
public:
    int grid_from_file( const QString filename, char grid[9][9] );
    int grid_to_file( const QString filename, char grid[9][9] );
    int grid_from_photofile( const QString filename, char grid[9][9] );
    int grid_from_image(QImage qim, char grid[9][9] );
    int grid_from_videocapture( char grid[9][9] );

private:
    bool is_valid_symbol(char x);
    int do_ocr(char grid[9][9]);
    int image_select_transform_preprocess();
    void showFigures();
    static void mouseHandler(int event, int x, int y, int, void* param);
    cv::VideoCapture cap;
};

#endif // IOGRID_H
