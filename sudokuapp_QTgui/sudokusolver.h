#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>
#include <QApplication>

#include <QLineEdit>
#include <QPushButton>
#include <QRegExpValidator>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QElapsedTimer>
#include <QStatusBar>

// if images are displayed
//#include <QGraphicsScene>
//#include <QGraphicsView>

#include "sudokuengine.h"
#include "iogrid.h"

class Sudokusolver : public QMainWindow
{
    Q_OBJECT

public:
    Sudokusolver(QWidget *parent = 0);

private:
    QLineEdit *inputfields[81];

    QPushButton *editgrid_b;
    QPushButton *cleargrid_b;
    QPushButton *solvegrid_b;
    QPushButton *check_solutions_b;
    QPushButton *input_txt_grid_b;
    QPushButton *output_txt_grid_b;
    QPushButton *getphotogrid_b;
    QPushButton *getclipboard_b;
    QPushButton *getvideoimg_b;
    QPushButton *generate_grid_b;

    QElapsedTimer timer;

    SudokuEngine engine;
    iogrid gridIO;

    bool editing_mode_on;

    void update_GUI_grid();
    void ShowMessage(QString msgText);

// For showing the image
//    QGraphicsView *view;
//    QGraphicsScene *scene;

private slots:
    void grid_edit();
    void grid_clear();
    void grid_solve();
    void grid_check_solvability();
    void grid_txt_input();
    void grid_txt_output();
    void grid_photo_input();
    void grid_clipboard_input();
    void grid_videoimg_input();
    void grid_generate();
};

#endif // SUDOKUSOLVER_H
