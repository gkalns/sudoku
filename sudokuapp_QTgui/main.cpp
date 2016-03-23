#include <QApplication>
#include "sudokusolver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Sudokusolver w;
    w.show();

    return a.exec();
}
