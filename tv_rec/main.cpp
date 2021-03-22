#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // QLocale locale(QLocale::Russian, QLocale::RussianFederation);

   MainWindow w;
   w.show();
   return a.exec();


    /*
    QCoreApplication a(argc, argv);
       QDateTime oldTime = QDateTime::currentDateTime();

       qDebug() << "before";
       qDebug() << QLocale::system().languageToString(QLocale::system().language());
       qDebug() << QLocale::system().countryToString(QLocale::system().country());
       qDebug() << oldTime.toString("dd MM yyyy");

       QLocale::setDefault(locale); // ключевой момент

       QDateTime newTime = QDateTime::currentDateTime();

       qDebug() << "after:";
       qDebug() << QLocale::system().languageToString(QLocale::system().language());
       qDebug() << QLocale::system().countryToString(QLocale::system().country());
       qDebug() << newTime.toString("MMM dddd");
       return a.exec();

       */
}
