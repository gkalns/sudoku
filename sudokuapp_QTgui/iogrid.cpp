#include "iogrid.h"

cv::Mat image;
cv::Point corner_rectangles[4];
int figure_selected = -1;


// if return 0 then fail, if return 1 then ok
int iogrid::grid_from_file(const QString filename, char grid[9][9])
{
    QFile file( filename );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    int i = 0;
    char x;

    while (!file.atEnd()) {
        if( !file.getChar(&x) ) return 0;

        if( is_valid_symbol(x) )
        {
            if( i == 81 )   // too long
            {
                file.close();
                return 0;
            }
            else
            {
                grid[i/9][i%9] = ( x == '-' ) ? '\0' : x;
                i++;
            }
        }
        else    // qt automatically translates \r\n to \n. If \n or space continue, else - sth wrong.
        {
            if( x == '\n' || x == ' ' ) continue;
            file.close();
            return 0;
        }
    }

    if( i != 81)    // too short
    {
        file.close();
        return 0;
    }

    file.close();
    return 1;
}

// if return 0 then fail, if return 1 then ok
int iogrid::grid_to_file( const QString filename, char grid[9][9] )
{
    QFile file( filename );
    //if( file.exists() ) return 0;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;

    for(int i = 0; i < 81; i++)
    {
        if( !(i % 9) && i ) file.putChar('\n');
        // if current position is \0 then write '-' otherwise corresponding number
        file.putChar( (grid[i/9][i%9]) ? grid[i/9][i%9] : '-' );
    }
    file.close();
    return 1;
}

// valid symbols for files are 1 - 9 and '-'
bool iogrid::is_valid_symbol(char x)
{
    if( (x >= '1' && x <= '9') || x == '-' ) return true;
    return false;
}


//// Photo input ////
// if return 0 - ok, if return 1 - warning, if return 2 - fail
int iogrid::grid_from_photofile( const QString filename, char grid[9][9] )
{
    image = cv::imread(filename.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    if( image.data == NULL ) return 2;

    image_select_transform_preprocess();
    return do_ocr(grid);
}

// if return 0 - ok, if return 1 - warning, if return 2 - fail
int iogrid::grid_from_image(QImage qim, char grid[9][9] )
{
    // always assuming 32bit colors
    if( qim.format() != QImage::Format_RGB32 ) return 2;
    cv::Mat image( qim.height(), qim.width(), CV_8UC4, (uchar*)qim.bits(), qim.bytesPerLine() );
    cv::cvtColor(image, image, CV_BGR2GRAY);

    image_select_transform_preprocess();
    return do_ocr(grid);
}

// if return 0 - ok, if return 1 - warning, if return 2 - fail, 3 - cant open camera
int iogrid::grid_from_videocapture( char grid[9][9] )
{
    cap.open(0); // open the default camera
    if(!cap.isOpened()) return 3;

    cv::namedWindow("Select the frame",1);
    for(;;)
    {
        cv::Mat frame;
        cap >> frame;
        cv::imshow("Select the frame", frame);
        if(cv::waitKey(30) >= 0){
            cap.release();
            cap.~VideoCapture();
            image = frame;
            cv::destroyWindow("Select the frame");
            break;
        }
    }
    cv::cvtColor(image, image, CV_BGR2GRAY);

    image_select_transform_preprocess();

    return do_ocr(grid);
}

// if return 0 - ok, if return 1 - warning, if return 2 - fail
int iogrid::do_ocr(char grid[9][9])
{
    bool warning_on = false;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) return 2;
    api->SetVariable("tessedit_char_whitelist", "123456789");

    api->SetImage( (const uchar*)image.data, image.cols, image.rows, 1, image.cols );
    cv::cvtColor(image, image, CV_GRAY2BGR);    // To mark colored rectangles image needs to be 3color

    int imagewidth = image.cols;
    int imageheight = image.rows;

    int tilesize_width = imagewidth / 9;
    int tilesize_height = imageheight / 9;

    int horizontal_gap = tilesize_width / 4;
    int vertical_gap = tilesize_height / 6;

    int horizontal_ocr_length = tilesize_width - 2 * horizontal_gap;
    int vertical_ocr_length = tilesize_height - 2 * vertical_gap;

    char *outText;
    for( int i = 0; i < 81; i++ ){
        int left_coord = (i % 9) * tilesize_width + horizontal_gap;
        int top_coord = (i / 9) * tilesize_height + vertical_gap;

        api->SetRectangle(left_coord, top_coord, horizontal_ocr_length, vertical_ocr_length);
        outText = api->GetUTF8Text();

        switch( (int)strlen(outText) ){
        case 0 :
            // empty space
            grid[i/9][i%9] = '\0';
            break;
        case 3 :
            // one character
            grid[i/9][i%9] = outText[0];
            cv::rectangle(image, cv::Point(left_coord,top_coord), \
                          cv::Point(left_coord+horizontal_ocr_length,top_coord+vertical_ocr_length),\
                          cv::Scalar(255,0,0), 1, 8, 0); // mark blue
            break;
        default :
            // suspected wrong result in ocr
            warning_on = true;
            grid[i/9][i%9] = '\0';
            cv::rectangle(image, cv::Point(left_coord,top_coord), \
                          cv::Point(left_coord+horizontal_ocr_length,top_coord+vertical_ocr_length),\
                          cv::Scalar(0,0,255), 1, 8, 0); // mark red - indicates that sth is wrong
        }
    }

    cv::imshow("Recognized numbers", image);

    api->End();
    delete [] outText;
    image.release();

    return warning_on;  // if warning_on - return 1, else 0
}

void iogrid::mouseHandler( int event, int x, int y, int, void * param )
{
    switch(event) {
        case CV_EVENT_LBUTTONDOWN:
            // determine which figure is selected
            for(int i = 0; i < 4; i++){
                if( x >= corner_rectangles[i].x-3 && x <= corner_rectangles[i].x+3 && y >= corner_rectangles[i].y-3 && y <= corner_rectangles[i].y+3){
                    figure_selected = i;
                    break;
                }
            }
            break;

        case CV_EVENT_LBUTTONUP:
            figure_selected = -1;	// release figure
            break;

        case CV_EVENT_MOUSEMOVE:
            if( figure_selected != -1 ){
                corner_rectangles[figure_selected].x = x;
                corner_rectangles[figure_selected].y = y;

                if( x > image.cols ) corner_rectangles[figure_selected].x = image.cols;
                if( x < 0 ) corner_rectangles[figure_selected].x = 0;
                if( y > image.rows ) corner_rectangles[figure_selected].y = image.rows;
                if( y < 0 ) corner_rectangles[figure_selected].y = 0;

                static_cast<iogrid*>(param)->showFigures();
            }
            break;
        }
}

int iogrid::image_select_transform_preprocess()
{
    // For selection shape
    cv::cvtColor(image, image, CV_GRAY2BGR);

    // Default window size is 600, scalable
    cv::namedWindow("image", cv::WINDOW_NORMAL);
    float coef = (float)image.rows / 600;
    cv::resizeWindow("image", image.cols/coef, 600);

    // initial corner positions
    corner_rectangles[0] = cv::Point(image.cols/4,image.rows/4);
    corner_rectangles[1] = cv::Point(image.cols*3/4,image.rows/4);
    corner_rectangles[2] = cv::Point(image.cols*3/4,image.rows*3/4);
    corner_rectangles[3] = cv::Point(image.cols/4,image.rows*3/4);

    showFigures();
    cv::setMouseCallback("image", mouseHandler, this );
    cv::waitKey(0);

    std::vector<cv::Point2f> corners;
    corners.push_back(corner_rectangles[0]);
    corners.push_back(corner_rectangles[1]);
    corners.push_back(corner_rectangles[2]);
    corners.push_back(corner_rectangles[3]);

    cv::Mat quad = cv::Mat::zeros(400, 400, CV_8UC3);

    std::vector<cv::Point2f> quad_pts;
    quad_pts.push_back(cv::Point2f(0, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
    quad_pts.push_back(cv::Point2f(0, quad.rows));

    cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
    cv::warpPerspective(image, quad, transmtx, quad.size());

    cv::destroyWindow("image");
    image.release();
    image = quad;
    cv::cvtColor(image, image, CV_BGR2GRAY);
    // preprocess
    cv::adaptiveThreshold(image, image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 21, 9);

    return 0;
}


void iogrid::showFigures()
{
    cv::Mat temp = image.clone();

    // fill
    const cv::Point* shape[1] = {corner_rectangles};
    int shape_n[1] = {4};
    cv::fillPoly(temp, shape, shape_n, 1, cv::Scalar(0, 180, 255));

    // alpha blend
    cv::addWeighted(temp, 0.5, image, 0.5, 0, temp);

    // corners not blended
    for(int i = 0; i < 4; i++){
        cv::rectangle(temp, corner_rectangles[i] - cv::Point(3,3), corner_rectangles[i] + cv::Point(3,3), cv::Scalar(0, 165, 255), -1, 8, 0);
    }

    // lines not blended
    cv::line(temp, corner_rectangles[0], corner_rectangles[1], cv::Scalar(0, 180, 255));
    cv::line(temp, corner_rectangles[1], corner_rectangles[2], cv::Scalar(0, 180, 255));
    cv::line(temp, corner_rectangles[2], corner_rectangles[3], cv::Scalar(0, 180, 255));
    cv::line(temp, corner_rectangles[3], corner_rectangles[0], cv::Scalar(0, 180, 255));

    cv::imshow("image", temp);
    temp.release();
}
