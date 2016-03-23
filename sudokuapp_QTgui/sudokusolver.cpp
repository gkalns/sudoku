#include "sudokusolver.h"

Sudokusolver::Sudokusolver(QWidget *parent)
    : QMainWindow(parent)
{
    // set up all inputfields
    for (int i = 0; i < 81; i++) {
        QLineEdit *lnedit = new QLineEdit();
        lnedit->setMaxLength(1);
        lnedit->setReadOnly(true);
        lnedit->setFixedHeight(40);
        lnedit->setFixedWidth(40);
        QFont font = lnedit->font();
        font.setPointSize(font.pointSize() + 8);
        lnedit->setFont(font);
        lnedit->setAlignment(Qt::AlignHCenter);
        QRegExp rx("[1-9]");
        lnedit->setValidator( new QRegExpValidator(rx, this) );
        inputfields[i] = lnedit;
    }

    editgrid_b = new QPushButton;
    editgrid_b->setText("Edit grid");
    connect(editgrid_b, SIGNAL(clicked()), this, SLOT(grid_edit()));

    solvegrid_b = new QPushButton;
    solvegrid_b->setText("Solve grid");
    connect(solvegrid_b, SIGNAL(clicked()), this, SLOT(grid_solve()));

    cleargrid_b = new QPushButton;
    cleargrid_b->setText("Clear grid");
    connect(cleargrid_b, SIGNAL(clicked()), this, SLOT(grid_clear()));

    getphotogrid_b = new QPushButton;
    getphotogrid_b->setText("Grid from photo");
    connect(getphotogrid_b, SIGNAL(clicked()), this, SLOT(grid_photo_input()));

    input_txt_grid_b = new QPushButton;
    input_txt_grid_b->setText("Grid from file");
    connect(input_txt_grid_b, SIGNAL(clicked()), this, SLOT(grid_txt_input()));

    output_txt_grid_b = new QPushButton;
    output_txt_grid_b->setText("Output grid");
    connect(output_txt_grid_b, SIGNAL(clicked()), this, SLOT(grid_txt_output()));

    check_solutions_b = new QPushButton;
    check_solutions_b->setText("Check solvability");
    connect(check_solutions_b, SIGNAL(clicked()), this, SLOT(grid_check_solvability()));

    getclipboard_b = new QPushButton;
    getclipboard_b->setText("Get clipboard grid");
    connect(getclipboard_b, SIGNAL(clicked()), this, SLOT(grid_clipboard_input()));

    getvideoimg_b = new QPushButton;
    getvideoimg_b->setText("Get Video image");
    connect(getvideoimg_b, SIGNAL(clicked()), this, SLOT(grid_videoimg_input()));

    generate_grid_b = new QPushButton;
    generate_grid_b->setText("Generate grid");
    connect(generate_grid_b, SIGNAL(clicked()), this, SLOT(grid_generate()));

    QGridLayout *squares[9];
    for( int i = 0; i < 9; i++ ){
        squares[i] = new QGridLayout;
        squares[i]->setHorizontalSpacing(2);
        squares[i]->setVerticalSpacing(2);
    }

    //mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    for (int i = 0; i < 81; i++) {
        int square_row = i / 27;
        int square_col = i / 3 % 3;
        int row = i / 9 % 3;
        int column = i % 3;
        squares[square_row*3+square_col]->addWidget(inputfields[i], row, column);
    }

    QWidget *container = new QWidget;
    QGridLayout *mainLayout = new QGridLayout;
    for(int i = 0; i < 9; i++){ mainLayout->addLayout(squares[i], i/3, i%3); }

    editing_mode_on = false;

    mainLayout->addWidget(editgrid_b,4,0,1,1);
    mainLayout->addWidget(cleargrid_b,4,1,1,1);
    mainLayout->addWidget(solvegrid_b,4,2,1,1);
    mainLayout->addWidget(getphotogrid_b,5,0,1,1);
    mainLayout->addWidget(input_txt_grid_b,5,1,1,1);
    mainLayout->addWidget(output_txt_grid_b,5,2,1,1);
    mainLayout->addWidget(check_solutions_b,6,0,1,1);
    mainLayout->addWidget(getclipboard_b,6,1,1,1);
    mainLayout->addWidget(getvideoimg_b,6,2,1,1);
    mainLayout->addWidget(generate_grid_b,7,0,1,1);

    container->setLayout(mainLayout);
    setCentralWidget(container);

    setWindowTitle("Sudoku Solver");
    statusBar()->showMessage("");
}


void Sudokusolver::grid_edit()
{
    if( editing_mode_on )
    {
        // read in grid to array
        for(int i = 0; i < 81; i++){ engine.grid[i/9][i%9] = inputfields[i]->text()[0].toLatin1(); }
        // check validity
        if( !engine.is_valid_grid() )
        {
            ShowMessage("Not valid grid!");
            return;
        }
        // disable inputs
        for (int i = 0; i < 81; i++) { inputfields[i]->setReadOnly(true); }

        editgrid_b->setText("Edit grid");
        editing_mode_on = false;
    }
    else
    {
        // enable inputs
        for (int i = 0; i < 81; i++) { inputfields[i]->setReadOnly(false); }

        editgrid_b->setText("Done");
        editing_mode_on = true;
    }
}

void Sudokusolver::grid_solve()
{
    timer.start();

    // check if valid grid (theoretically shouldn't get here, all checks need to be done just after loading grid)
    if( !engine.is_valid_grid() )
    {
        ShowMessage("Not valid grid! Conflicting numbers.");
        return;
    }
    // solve grid
    if( !engine.dumbsolve() )
    {
        ShowMessage("No solutions!");
        return;
    }
    // output result to GUI
    update_GUI_grid();
    statusBar()->showMessage( QString("Elapsed time: %1 ms").arg(QString::number(timer.elapsed())) );
}

void Sudokusolver::grid_clear()
{
    for(int i = 0; i < 81; i++){ inputfields[i]->setText(""); }
}

void Sudokusolver::grid_photo_input()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if( filename.isEmpty() ) return;    // no file is selected

    switch(gridIO.grid_from_photofile(filename, engine.grid))
    {
    case 0 :    // everything ok
        break;
    case 1 :
        ShowMessage("There might be missing characters!");
        break;
    default :
        ShowMessage("Error in OCR or not correct file!");
        return;
    }

    // grid to GUI
    update_GUI_grid();
}

void Sudokusolver::grid_txt_input()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open pzl file"), "", tr("Puzzle Files (*.pzl)"));
    if( filename.isEmpty() ) return;    // no file is selected

    if( !gridIO.grid_from_file(filename, engine.grid) )
    {
        ShowMessage("Not valid grid or can't read the file!");
        return;
    }
    update_GUI_grid();
}

void Sudokusolver::grid_txt_output()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save File"), "", tr("Solution files (*.sol)"));
    if( filename.isEmpty() ) return;    // no file is selected, cancelled
    if (!filename.endsWith(".sol")) filename += ".sol";

    if( !gridIO.grid_to_file( filename, engine.grid ) )
    {
        ShowMessage("Can't write to file!");
        return;
    }
}

void Sudokusolver::grid_check_solvability()
{
    timer.start();
    int res = engine.solution_count();
    statusBar()->showMessage( QString("Elapsed time: %1 ms").arg(QString::number(timer.elapsed())) );
    switch(res)
    {
    case 0 :
        ShowMessage("No solutions!");
        break;
    case 1 :
        ShowMessage("One solution!");
        break;
    case 2 :
        ShowMessage("More than 1 solution!");
        break;
    default :
        ShowMessage("Oops, sth went wrong!");
    }
}


void Sudokusolver::grid_clipboard_input()
{
    const QClipboard *clipboard = QApplication::clipboard();
    QPixmap image_pxm = clipboard->pixmap();

    if( image_pxm.isNull() )
    {
        ShowMessage("Clipboard is not an image!");
        return;
    }

    switch(gridIO.grid_from_image(image_pxm.toImage(), engine.grid))
    {
    case 0 :    // everything ok
        break;
    case 1 :
        ShowMessage("There might be missing characters!");
        break;
    default :
        ShowMessage("Error in OCR or cannot load the image!");
        return;
    }

    // grid to GUI
    update_GUI_grid();

    // For viewing clipboard image
//    scene = new QGraphicsScene;
//    view = new QGraphicsView(scene);
//    scene->addPixmap(image_pxm);
//    view->show();
}


void Sudokusolver::update_GUI_grid()
{
    for(int i = 0; i < 81; i++)
    {
        inputfields[i]->setText( (engine.grid[i/9][i%9] == '\0') ? "" : QString(engine.grid[i/9][i%9]) );
    }
}

void Sudokusolver::ShowMessage(QString msgText)
{
    QMessageBox msgBox;
    msgBox.setText(msgText);
    msgBox.exec();
}

void Sudokusolver::grid_videoimg_input()
{
    switch(gridIO.grid_from_videocapture(engine.grid))
    {
    case 0 :    // everything ok
        break;
    case 1 :
        ShowMessage("There might be missing characters!");
        break;
    case 3 :
        ShowMessage("Can't open video stream!");
        return;
    default :
        ShowMessage("Error in OCR or cannot load the image!");
        return;
    }
    // grid to GUI
    update_GUI_grid();

}

void Sudokusolver::grid_generate(){

    timer.start();
    engine.generate();
    update_GUI_grid();
    statusBar()->showMessage( QString("Elapsed time: %1 ms").arg(QString::number(timer.elapsed())) );
}
