//#include "dialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Dialog dialog;
//    dialog.show();
    QFile qss(":/QSS/myStyle.qss");
    if( qss.open(QFile::ReadOnly))
    {
        qDebug("open QSS file success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug("Open QSS file failed");
    }
    MainWindow w;
    w.show();
    return a.exec();
}
