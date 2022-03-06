//#include "dialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Dialog dialog;
//    dialog.show();
    MainWindow w;
    w.show();
    return a.exec();
}
