#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("./../data_files/icon2.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
